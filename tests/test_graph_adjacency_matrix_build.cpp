#include "traffic_graph.h"
#include "assert.h"
#include <iostream>

#define NUMBER_OF_VERTICES 4
#define EDGE1 {0, 1}
#define EDGE1_WEIGHT 3
#define EDGE2 {2, 3}
#define EDGE2_WEIGHT 6

using namespace assertion;
using namespace ufrrj;
int main (void) {

	TrafficGraphBuilder graphBuilder;
	TrafficGraph* graph;
	TrafficGraph::Edge	edge1 = EDGE1,
						edge2 = EDGE2,
						reverseEdge1 = {edge1.vertice2, edge1.vertice1},
						reverseEdge2 = {edge2.vertice2, edge2.vertice1};
	try {

		graphBuilder.addEdgeBetween(edge1, EDGE1_WEIGHT);
		graphBuilder.addEdgeBetween(edge2, EDGE1_WEIGHT);

		graph = graphBuilder.buildAsAdjacencyMatrix();

		assert(graph->getNumberOfVertices(), equals<size_t>(), (size_t)NUMBER_OF_VERTICES);

		assert(graph->weight(edge1), equals<int>(), EDGE1_WEIGHT);
		assert(graph->weight(reverseEdge1), equals<int>(), EDGE1_WEIGHT);

		assert(graph->weight(edge2), equals<int>(), EDGE2_WEIGHT);
		assert(graph->weight(reverseEdge2), equals<int>(), EDGE2_WEIGHT);

		for (size_t vertice1 = 0; vertice1 < NUMBER_OF_VERTICES; vertice1++) {
			for (size_t vertice2 = 0; vertice2 < NUMBER_OF_VERTICES; vertice2++) {
				TrafficGraph::Edge currentEdge = {vertice1, vertice2};
				if	(currentEdge == edge1 || currentEdge == reverseEdge1) {
					continue;
				} else {
					assert(graph->weight(currentEdge), equals<int>(), 0);
				}
			}
		}
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		delete graph;
		return 1;
	}

	return 0;
}