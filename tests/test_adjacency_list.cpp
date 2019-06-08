#include "traffic_graph.h"
#include "assert.h"

#define NUMBER_OF_VERTICES 8
#define EDGE1 {7, 5}
#define EDGE1_WEIGHT 6
#define EDGE2 {2, 3}
#define EDGE2_WEIGHT 3
#define CYCLE 20
#define TIMING_U 16
#define TIMING_V 8

using namespace traffic;
int main (void) {

	GraphBuilder* graphBuilder;
	Graph* graph;
	Graph::Edge	edge1 = EDGE1,
						edge2 = EDGE2,
						reverseEdge1 = {edge1.vertice2, edge1.vertice1},
						reverseEdge2 = {edge2.vertice2, edge2.vertice1};

	test_case("build adjacency list") {
		graphBuilder = new GraphBuilder();
		graphBuilder->addEdge(edge1, EDGE1_WEIGHT);
		graphBuilder->addEdge(edge2, EDGE2_WEIGHT);
		graphBuilder->withCycle(CYCLE);
		graph = graphBuilder->buildAsAdjacencyList();
		assert_true(graph != NULL);
	} end_test_case;

	test_case("has correct dimensions") {
		assert_equal(graph->getNumberOfVertices(), NUMBER_OF_VERTICES);
	} end_test_case;

	test_case("has correct cycle") {
		assert_equal(graph->getCycle(), CYCLE);
	} end_test_case;

	test_case("first added edge has correct weight") {
		assert_equal(graph->weight(edge1), EDGE1_WEIGHT);
	} end_test_case;

	test_case("second added edge has correct weight") {
		assert_equal(graph->weight(edge2), EDGE2_WEIGHT);
	} end_test_case;

	test_case("weight for edge (u,v) equals weight for edge (v,u) for both added edges") {
		assert_equal(graph->weight(reverseEdge1), EDGE1_WEIGHT);
		assert_equal(graph->weight(reverseEdge2), EDGE2_WEIGHT);
	} end_test_case;

	test_case("non existing edges have weight -1") {
		for (size_t vertice1 = 0; vertice1 < NUMBER_OF_VERTICES; vertice1++) {
			for (size_t vertice2 = 0; vertice2 < NUMBER_OF_VERTICES; vertice2++) {
				Graph::Edge currentEdge = {vertice1, vertice2};
				if	(currentEdge == edge1 || currentEdge == reverseEdge1 || currentEdge == edge2 || currentEdge == reverseEdge2) {
					continue;
				} else {
					assert_equal(graph->weight(currentEdge), -1);
				}
			}
		}
	} end_test_case;

	test_case("set vertice timing") {
		graph->setTiming(edge1.vertice1, TIMING_U);
		graph->setTiming(edge1.vertice2, TIMING_V);

		assert_equal(graph->getTiming(edge1.vertice1), TIMING_U);
		assert_equal(graph->getTiming(edge1.vertice2), TIMING_V);
	} end_test_case;

	test_case("penalty between two vertices with edge between them") {
		int penalty_uv = graph->penalty(edge1.vertice1, edge1.vertice2);
		int penalty_vu = graph->penalty(edge1.vertice2, edge1.vertice1);
		assert_equal(penalty_uv, 6);
		assert_equal(penalty_vu, 2);
	} end_test_case;

	test_case ("penalty between two vertices with no edge between them") {
		int penalty = graph->penalty(4, 3);
		assert_equal(penalty, 0);
	} end_test_case;

	test_case ("destroy GraphBuilder") {
		delete graphBuilder;
	} end_test_case;

	test_case("destroy AdjacencyMatrixGraph") {
		delete graph;
	} end_test_case;
}
