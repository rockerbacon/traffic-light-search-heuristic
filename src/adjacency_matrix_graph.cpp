#include "traffic_graph.h"

using namespace traffic;

AdjacencyMatrixGraph::AdjacencyMatrixGraph (Weight *adjacencyMatrix, size_t numberOfVertices, TimeUnit cycle) : Graph(numberOfVertices, cycle) {
	this->adjacencyMatrix = adjacencyMatrix;
	this->matrixDimensionX2minus1 = numberOfVertices*2-1;
}

AdjacencyMatrixGraph::~AdjacencyMatrixGraph (void) {
	delete this->adjacencyMatrix;
}

Weight AdjacencyMatrixGraph::weight (const Graph::Edge& edge) const {
	Weight weight;
	Vertice i, j, index;
	if (edge.vertice1 > edge.vertice2) {
		i = edge.vertice2;
		j = edge.vertice1;
	} else if (edge.vertice1 < edge.vertice2) {
		i = edge.vertice1;
		j = edge.vertice2;
	} else {
		return -1;
	}

	index = j + i*(this->matrixDimensionX2minus1 - i)/2;
	return this->adjacencyMatrix[index];
}
