#include "traffic_graph/traffic_graph.h"
#include "assertions/assert.h"

#define NUMBER_OF_VERTICES 8
#define EDGE1 {7, 5}
#define EDGE1_WEIGHT 6
#define EDGE2 {2, 3}
#define EDGE2_WEIGHT 3
#define EDGE3 {2, 4}
#define EDGE3_WEIGHT 13
#define EDGE4 {2, 0}
#define EDGE4_WEIGHT 16
#define CYCLE 20
#define TIMING_U 16
#define TIMING_V 8

using namespace traffic;
int main (void) {

	GraphBuilder* graphBuilder;
	Graph* graph;
	Graph::Edge	edge1 = EDGE1,
				reverseEdge1 = {edge1.vertex2, edge1.vertex1},
				edge2 = EDGE2,
				reverseEdge2 = {edge2.vertex2, edge2.vertex1},
				edge3 = EDGE3,
				reverseEdge3 = {edge3.vertex2, edge3.vertex1},
				edge4 = EDGE4,
				reverseEdge4 = {edge4.vertex2, edge4.vertex1};

	test_case("build adjacency matrix") {
		graphBuilder = new GraphBuilder();
		graphBuilder->addEdge(edge1, EDGE1_WEIGHT);
		graphBuilder->addEdge(edge2, EDGE2_WEIGHT);
		graphBuilder->addEdge(edge3, EDGE3_WEIGHT);
		graphBuilder->addEdge(edge4, EDGE4_WEIGHT);
		graphBuilder->withCycle(CYCLE);
		graph = graphBuilder->buildAsAdjacencyMatrix();
		assert_true(graph != NULL);
	} end_test_case;

	test_case("first added edge has correct weight") {
		assert_equal(graph->weight(edge1), EDGE1_WEIGHT);
	} end_test_case;

	test_case("second added edge has correct weight") {
		assert_equal(graph->weight(edge2), EDGE2_WEIGHT);
	} end_test_case;

	test_case("weight for edge (u,v) equals weight for edge (v,u)") {
		assert_equal(graph->weight(reverseEdge1), EDGE1_WEIGHT);
		assert_equal(graph->weight(reverseEdge2), EDGE2_WEIGHT);
	} end_test_case;

	test_case("non existing edges have weight -1") {
		for (Vertex vertex1 = 0; vertex1 < NUMBER_OF_VERTICES; vertex1++) {
			for (Vertex vertex2 = 0; vertex2 < NUMBER_OF_VERTICES; vertex2++) {
				Graph::Edge currentEdge = {vertex1, vertex2};
				if	(
						currentEdge == edge1 || currentEdge == reverseEdge1 || currentEdge == edge2 || currentEdge == reverseEdge2 ||
						currentEdge == edge3 || currentEdge == reverseEdge3 || currentEdge == edge4 || currentEdge == reverseEdge4
					) {
					continue;
				} else {
					assert_equal(graph->weight(currentEdge), -1);
				}
			}
		}
	} end_test_case;

	const std::unordered_map<Vertex, Weight>* neighbors;

	test_case("asking for neighborhood raises no errors") {
		neighbors = &graph->neighborsOf(2);
	} end_test_case;

	test_case("neighborhood has correct size") {
		assert_equal(neighbors->size(), 3);
	} end_test_case;

	test_case("neighborhood has all vertices with correct edge weights") {
		bool found0, found3, found4;

		for (auto it: *neighbors) {
			if (it.first == 0) {
				found0 = true;
				assert_equal(it.second, EDGE4_WEIGHT);
			} else if (it.first == 3) {
				found3 = true;
				assert_equal(it.second, EDGE2_WEIGHT);
			} else if (it.first == 4) {
				found4 = true;
				assert_equal(it.second, EDGE3_WEIGHT);
			}
		}

		assert_true(found0 && found3 && found4);
	} end_test_case;

	test_case("if u is in the neighborhood of v then v is also in the neighborhood of u") {
		auto neighbors3 = graph->neighborsOf(3);
		assert_true(neighbors->find(3) != neighbors->end());
		assert_true(neighbors3.find(2) != neighbors3.end());
	} end_test_case;

	test_case ("destroy GraphBuilder") {
		delete graphBuilder;
	} end_test_case;

	test_case("destroy AdjacencyMatrixGraph") {
		delete graph;
	} end_test_case;
}
