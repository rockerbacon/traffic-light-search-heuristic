#include "traffic_graph.h"

using namespace traffic;
using namespace std;

AdjacencyMatrixGraph::AdjacencyMatrixGraph (Weight *adjacencyMatrix, size_t numberOfVertices, TimeUnit cycle) : Graph(numberOfVertices, cycle) {
	this->adjacencyMatrix = adjacencyMatrix;
	this->matrixDimensionX2minus1 = numberOfVertices*2-1;
}

AdjacencyMatrixGraph::~AdjacencyMatrixGraph (void) {
	delete this->adjacencyMatrix;
	for (auto it : this->neighborhoodRequests) {
		delete it.second;
	}
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

const unordered_map<Vertice, Weight>& AdjacencyMatrixGraph::neighborsOf (Vertice vertice) const {
	Weight weight;
	Vertice vertice2;
	Edge edge;
	unordered_map<Vertice, Weight>* neighborhood;
	unordered_map<Vertice, unordered_map<Vertice, Weight>*>::iterator it;

	it = this->neighborhoodRequests.find(vertice);
	if (it != this->neighborhoodRequests.end()) {
		neighborhood = it->second;
	} else {
		neighborhood = new unordered_map<Vertice, Weight>();

		for (vertice2 = 0; vertice2 < this->getNumberOfVertices(); vertice2++) {
			edge = {vertice, vertice2};
			weight = this->weight(edge);
			if (weight != -1) {
				(*neighborhood)[vertice2] = this->weight(edge);
			}
		}

		this->neighborhoodRequests[vertice] = neighborhood;
	}

	return *neighborhood;
}
