#include "assert.h"
#include "traffic_graph.h"

#define NUMBER_OF_VERTICES 8
#define EDGE1 {7, 5}
#define EDGE1_WEIGHT 6
#define EDGE2 {5, 2}
#define EDGE2_WEIGHT 10
#define EDGE3 {5, 4}
#define EDGE3_WEIGHT 4
#define CYCLE 20
#define TIMING_7 16
#define TIMING_5 8
#define TIMING_2 4
#define TIMING_4 7

using namespace traffic;
using namespace std;

Graph::Edge	edge1 = EDGE1,
			edge2 = EDGE2,
			edge3 = EDGE3;

class MockGraphImplementation : public Graph {
	private:
		unordered_map<Vertex, Weight> neighborsOf2;
		unordered_map<Vertex, Weight> neighborsOf4;
		unordered_map<Vertex, Weight> neighborsOf5;
		unordered_map<Vertex, Weight> neighborsOf7;
		unordered_map<Vertex, Weight> emptyMap;
	public:
		MockGraphImplementation() : Graph(NUMBER_OF_VERTICES, CYCLE) {
			this->neighborsOf2 = { {5, 10} };
			this->neighborsOf4 = { {5, 4} };
			this->neighborsOf5 = { {2, 10}, {4, 4}, {7, 6} };
			this->neighborsOf7 = { {5, 6} };
		}

		virtual Weight weight(const Graph::Edge& edge) const {

			if (edge == edge1) {
				return EDGE1_WEIGHT;
			} else if (edge == edge2) {
				return EDGE2_WEIGHT;
			} else if (edge == edge3) {
				return EDGE3_WEIGHT;
			} else {
				return -1;
			}
		}

		virtual const unordered_map<Vertex, Weight>& neighborsOf (Vertex vertex) const {
			switch(vertex) {
				case 2:
					return this->neighborsOf2;
					break;
				case 4:
					return this->neighborsOf4;
					break;
				case 5:
					return this->neighborsOf5;
					break;
				case 7:
					return this->neighborsOf7;
					break;
				default:
					return this->emptyMap;
			}
		}
};

int main (void) {
	Graph* graph;
	Graph::Edge edge1 = EDGE1,
				edge2 = EDGE2,
				edge3 = EDGE3;

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

	test_case("set vertices timings") {
		graph->setTiming(7, TIMING_7);
		graph->setTiming(5, TIMING_5);
		graph->setTiming(2, TIMING_2);
		graph->setTiming(4, TIMING_4);
		assert_equal(graph->getTiming(edge1.vertex1), TIMING_7);
		assert_equal(graph->getTiming(edge1.vertex2), TIMING_5);
	} end_test_case;

	test_case("edge (u,v) equals edge (v,u)") {
		Graph::Edge a, b;
		a = {1, 2};
		b = {2, 1};
		assert_true(a == b);
	} end_test_case;

	test_case("penalty between two vertices with edge between them") {
		TimeUnit penalty_uv = graph->penalty(edge1.vertex1, edge1.vertex2);
		TimeUnit penalty_vu = graph->penalty(edge1.vertex2, edge1.vertex1);
		assert_equal(penalty_uv, 6);
		assert_equal(penalty_vu, 2);
	} end_test_case;

	test_case ("penalty between two vertices with no edge between them is 0") {
		TimeUnit penalty;
		Graph::Edge edge;
		for (size_t i = 0; i < NUMBER_OF_VERTICES; i++) {
			for (size_t j = 0; j < NUMBER_OF_VERTICES; j++) {
				edge = {i, j};
				if (edge == edge1 || edge == edge2 || edge == edge3) {
					continue;
				}
				penalty = graph->penalty(i, j);
				assert_equal(penalty, 0);
			}
		}
	} end_test_case;

	test_case ("total penalty for a vertex") {
		TimeUnit penalty;
		TimeUnit expectedPenalty = 2+6+5 + 6+6+3;
		penalty = graph->vertexPenalty(5);
		assert_equal(penalty, expectedPenalty);
	} end_test_case;

	test_case ("total penalty for a vertex ignoring neighbors' penalties") {
		TimeUnit penalty;
		TimeUnit expectedPenalty = 2+6+5;
		penalty = graph->vertexPenaltyOnewayOnly(5);
		assert_equal(penalty, expectedPenalty);
	} end_test_case;

	test_case ("total penalty for the graph") {
		TimeUnit penalty;
		TimeUnit expectedPenalty = 28;
		penalty = graph->totalPenalty();
		assert_equal(penalty, expectedPenalty);
	} end_test_case;

	delete graph;
}
