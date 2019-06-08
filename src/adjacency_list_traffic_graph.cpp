#include "traffic_graph.h"

using namespace ufrrj;
using namespace std;

AdjacencyListTrafficGraph::AdjacencyListTrafficGraph(unordered_map<Vertice, int>* adjacencyList, size_t numberOfVertices, int cycle) {
	this->adjacencyList = adjacencyList;
	this->numberOfVertices = numberOfVertices;
	this->cycle = cycle;
}

AdjacencyListTrafficGraph::~AdjacencyListTrafficGraph(void) {
	delete this->adjacencyList;
}

int AdjacencyListTrafficGraph::weight(const Edge& edge) const {
	int weight;
	unordered_map<Vertice, int>* vertice1AdjacencyMap;
	unordered_map<Vertice, int>::iterator it;
	Vertice i, j;

	if (edge.vertice1 > edge.vertice2) {
		i = edge.vertice2;
		j = edge.vertice1;
	} else {
		i = edge.vertice1;
		j = edge.vertice2;
	}

	if (i < this->numberOfVertices-1) {
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

size_t AdjacencyListTrafficGraph::getNumberOfVertices (void) const {
	return this->numberOfVertices;
}

void AdjacencyListTrafficGraph::setTiming (Vertice vertice, int timing) {

}

int AdjacencyListTrafficGraph::getTiming (Vertice vertice) const {
	return 0;
}

int AdjacencyListTrafficGraph::penalty (Vertice vertice1, Vertice vertice2) const {
	return 0;
}

int AdjacencyListTrafficGraph::getCycle (void) const {
	return this->cycle;
}
