#include "traffic_graph.h"

using namespace traffic;
using namespace std;

AdjacencyListGraph::AdjacencyListGraph(unordered_map<Vertice, Weight>* adjacencyList, size_t numberOfVertices, TimeUnit cycle) : Graph(numberOfVertices, cycle) {
	this->adjacencyList = adjacencyList;
}

AdjacencyListGraph::~AdjacencyListGraph(void) {
	delete this->adjacencyList;
}

Weight AdjacencyListGraph::weight(const Edge& edge) const {
	Weight weight;
	unordered_map<Vertice, Weight>* vertice1AdjacencyMap;
	unordered_map<Vertice, Weight>::iterator it;
	Vertice i, j;

	if (edge.vertice1 > edge.vertice2) {
		i = edge.vertice2;
		j = edge.vertice1;
	} else {
		i = edge.vertice1;
		j = edge.vertice2;
	}

	if (i < this->getNumberOfVertices()-1) {
		vertice1AdjacencyMap = this->adjacencyList + i;
		it = vertice1AdjacencyMap->find(j);
		if (it != vertice1AdjacencyMap->end()) {
			weight = it->second;
		} else {
			weight = -1;
		}
	} else {
		weight = -1;
	}
	return weight;
}

const unordered_map<Vertice, Weight>& AdjacencyListGraph::neighborsOf(Vertice vertice) const {
	return *(this->adjacencyList + vertice);
}
