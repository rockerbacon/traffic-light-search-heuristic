#include "traffic_graph.h"

using namespace traffic;
using namespace std;

AdjacencyMatrixGraph::AdjacencyMatrixGraph (Weight *adjacencyMatrix, Vertex numberOfVertices, TimeUnit cycle) : Graph(numberOfVertices, cycle) {
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
	Vertex i, j, index;
	if (edge.vertex1 > edge.vertex2) {
		i = edge.vertex2;
		j = edge.vertex1;
	} else if (edge.vertex1 < edge.vertex2) {
		i = edge.vertex1;
		j = edge.vertex2;
	} else {
		return -1;
	}

	index = j + i*(this->matrixDimensionX2minus1 - i)/2;
	return this->adjacencyMatrix[index];
}

const unordered_map<Vertex, Weight>& AdjacencyMatrixGraph::neighborsOf (Vertex vertex) const {
	Weight weight;
	Vertex vertex2;
	Edge edge;
	unordered_map<Vertex, Weight>* neighborhood;
	unordered_map<Vertex, unordered_map<Vertex, Weight>*>::iterator it;

	it = this->neighborhoodRequests.find(vertex);
	if (it != this->neighborhoodRequests.end()) {
		neighborhood = it->second;
	} else {
		neighborhood = new unordered_map<Vertex, Weight>();

		for (vertex2 = 0; vertex2 < this->getNumberOfVertices(); vertex2++) {
			edge = {vertex, vertex2};
			weight = this->weight(edge);
			if (weight != -1) {
				(*neighborhood)[vertex2] = this->weight(edge);
			}
		}

		this->neighborhoodRequests[vertex] = neighborhood;
	}

	return *neighborhood;
}
