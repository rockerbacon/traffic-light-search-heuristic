#include "assert.h"
#include "traffic_graph.h"

#define NUMBER_OF_VERTICES 8
#define EDGE1 {7, 5}
#define EDGE1_WEIGHT 6
#define CYCLE 20
#define TIMING_U 16
#define TIMING_V 8

using namespace traffic;

class MockGraphImplementation : public Graph {
	public:
		MockGraphImplementation() : Graph(NUMBER_OF_VERTICES, CYCLE) {}
		virtual Weight weight(const Graph::Edge& edge) const {
			Edge edge1 = EDGE1;
			if (edge == edge1) {
				return EDGE1_WEIGHT;
			} else {
				return -1;
			}
		}
};

int main (void) {
	Graph* graph;
	Graph::Edge edge1 = EDGE1;

	test_case("constructor raises no error") {
		graph = new MockGraphImplementation();
		assert_true(graph != NULL);
	} end_test_case;

	test_case("has correct number of vertices") {
		assert_equal(graph->getNumberOfVertices(), NUMBER_OF_VERTICES);
	} end_test_case;

	test_case("has correct cycle") {
		assert_equal(graph->getCycle(), CYCLE);
	} end_test_case;

	test_case("set vertice timings") {
		graph->setTiming(edge1.vertice1, TIMING_U);
		graph->setTiming(edge1.vertice2, TIMING_V);
		assert_equal(graph->getTiming(edge1.vertice1), TIMING_U);
		assert_equal(graph->getTiming(edge1.vertice2), TIMING_V);
	} end_test_case;

	test_case("penalty between two vertices with edge between them") {
		TimeUnit penalty_uv = graph->penalty(edge1.vertice1, edge1.vertice2);
		TimeUnit penalty_vu = graph->penalty(edge1.vertice2, edge1.vertice1);
		assert_equal(penalty_uv, 6);
		assert_equal(penalty_vu, 2);
	} end_test_case;

	test_case ("penalty between two vertices with no edge between them is 0") {
		TimeUnit penalty;
		Graph::Edge edge;
		for (size_t i = 0; i < NUMBER_OF_VERTICES; i++) {
			for (size_t j = 0; j < NUMBER_OF_VERTICES; j++) {
				edge = {i, j};
				if (edge == edge1) {
					continue;
				}
				penalty = graph->penalty(i, j);
				assert_equal(penalty, 0);
			}
		}
	} end_test_case;

	delete graph;
}
