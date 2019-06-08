#include "traffic_graph.h"

using namespace traffic;

AdjacencyMatrixGraph::AdjacencyMatrixGraph (Weight *adjacencyMatrix, size_t numberOfVertices, TimeUnit cycle) : Graph(numberOfVertices, cycle) {
	this->adjacencyMatrix = adjacencyMatrix;
}

AdjacencyMatrixGraph::~AdjacencyMatrixGraph (void) {
	delete this->adjacencyMatrix;
}

Weight AdjacencyMatrixGraph::weight (const Graph::Edge& edge) const {
	size_t i, j;
	if (edge.vertice1 > edge.vertice2) {
		i = edge.vertice2;
		j = edge.vertice1;
	} else {
		i = edge.vertice1;
		j = edge.vertice2;
	}
	return this->adjacencyMatrix[i*this->getNumberOfVertices() + j];
}
