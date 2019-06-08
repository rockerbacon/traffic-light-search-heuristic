#include "traffic_graph.h"
#include "assert.h"

#define NUMBER_OF_VERTICES 4
#define EDGE1 {0, 1}
#define EDGE1_WEIGHT 3
#define EDGE2 {2, 3}
#define EDGE2_WEIGHT 6
#define CYCLE 10

using namespace ufrrj;
int main (void) {

	TrafficGraphBuilder* graphBuilder;
	TrafficGraph* graph;
	TrafficGraph::Edge	edge1 = EDGE1,
						edge2 = EDGE2,
						reverseEdge1 = {edge1.vertice2, edge1.vertice1},
						reverseEdge2 = {edge2.vertice2, edge2.vertice1};

	test_case("TrafficGraphBuilder instantiation raises no errors") {
		graphBuilder = new TrafficGraphBuilder();
	} end_test_case;

	test_case("adding edge to TrafficGraphBuilder raises no errors") {
		graphBuilder->addEdge(edge1, EDGE1_WEIGHT);
		graphBuilder->addEdge(edge2, EDGE2_WEIGHT);
	} end_test_case;

	test_case("setting cycle to TrafficGraphBuilder raises no errors") {
		graphBuilder->withCycle(CYCLE);
	} end_test_case;

	test_case("build as adjacency matrix raises no errors") {
		graph = graphBuilder->buildAsAdjacencyMatrix();
	} end_test_case;

	test_case("adjacency matrix has correct dimensions") {
		assert_equal(graph->getNumberOfVertices(), NUMBER_OF_VERTICES);
	} end_test_case;

	test_case("adjacency matrix has correct cycle") {
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
				TrafficGraph::Edge currentEdge = {vertice1, vertice2};
				if	(currentEdge == edge1 || currentEdge == reverseEdge1 || currentEdge == edge2 || currentEdge == reverseEdge2) {
					continue;
				} else {
					assert_equal(graph->weight(currentEdge), -1);
				}
			}
		}
	} end_test_case;

	test_case ("TrafficGraphBuilder destruction throws no errors") {
		delete graphBuilder;
	} end_test_case;

	test_case("AdjacencyMatrixGraph destruction throws no errors") {
		delete graph;
	} end_test_case;
}
