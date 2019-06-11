#include "traffic_graph.h"
#include "assert.h"

#define NUMBER_OF_VERTICES 30
//#define MIN_VERTEX_DEGREE 2
#define MAX_VERTEX_DEGREE 3
#define MIN_EDGE_WEIGHT 2
#define MAX_EDGE_WEIGHT 13
#define CYCLE 20
#define TIMING_U 16
#define TIMING_V 8

using namespace traffic;
int main (void) {

	GraphBuilder* graphBuilder;
	Graph* graph;

/*
	Graph::Edge	edge1 = EDGE1,
						edge2 = EDGE2,
						reverseEdge1 = {edge1.vertice2, edge1.vertice1},
						reverseEdge2 = {edge2.vertice2, edge2.vertice1};
*/
	test_case("build random adjacency list") {
		graphBuilder = new GraphBuilder();
		graphBuilder->generateRandomGraph(NUMBER_OF_VERTICES, MAX_VERTEX_DEGREE, MIN_EDGE_WEIGHT, MAX_EDGE_WEIGHT);
		graphBuilder->withCycle(CYCLE);
		graph = graphBuilder->buildAsAdjacencyList();
		assert_true(graph != NULL);
	} end_test_case;

	/*
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

	*/
	test_case ("destroy GraphBuilder") {
		delete graphBuilder;
	} end_test_case;

	test_case("destroy AdjacencyMatrixGraph") {
		delete graph;
	} end_test_case;
}
