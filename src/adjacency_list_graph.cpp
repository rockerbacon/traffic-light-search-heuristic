#include "traffic_graph.h"

using namespace traffic;
using namespace std;

AdjacencyListGraph::AdjacencyListGraph(unordered_map<Vertex, Weight>* adjacencyList, size_t numberOfVertices, TimeUnit cycle) : Graph(numberOfVertices, cycle) {
	this->adjacencyList = adjacencyList;
}

AdjacencyListGraph::~AdjacencyListGraph(void) {
	delete [] this->adjacencyList;
}

Weight AdjacencyListGraph::weight(const Edge& edge) const {
	Weight weight;
	unordered_map<Vertex, Weight>* vertex1AdjacencyMap;
	unordered_map<Vertex, Weight>::iterator it;
	Vertex i, j;

	if (edge.vertex1 > edge.vertex2) {
		i = edge.vertex2;
		j = edge.vertex1;
	} else {
		i = edge.vertex1;
		j = edge.vertex2;
	}

	if (i < this->getNumberOfVertices()-1) {
		vertex1AdjacencyMap = this->adjacencyList + i;
		it = vertex1AdjacencyMap->find(j);
		if (it != vertex1AdjacencyMap->end()) {
			weight = it->second;
		} else {
			weight = -1;
		}
	} else {
		weight = -1;
	}
	return weight;
}

const unordered_map<Vertex, Weight>& AdjacencyListGraph::neighborsOf(Vertex vertex) const {
	return *(this->adjacencyList + vertex);
}
