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
	unordered_map<Vertice, int>& vertice1AdjacencyMap = this->adjacencyList[edge.vertice1];
	unordered_map<Vertice, int>::iterator it = vertice1AdjacencyMap.find(edge.vertice2);
	if (it != vertice1AdjacencyMap.end()) {
		weight = it->second;
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
