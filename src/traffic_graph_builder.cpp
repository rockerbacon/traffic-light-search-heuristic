#include "traffic_graph.h"

using namespace ufrrj;
using namespace std;
TrafficGraphBuilder::TrafficGraphBuilder () {
	this->highestVerticeIndex = 0;
}

TrafficGraphBuilder::~TrafficGraphBuilder (void) {
	for (auto mapIterator = this->adjacencyListMap.begin(); mapIterator != this->adjacencyListMap.end(); mapIterator++) {
		delete(mapIterator->second);
	}
}

void TrafficGraphBuilder::addEdge(const TrafficGraph::Edge& edge, int weight) {
	decltype(TrafficGraphBuilder::adjacencyListMap)::iterator vertice1Index;
	unordered_map<Vertice, int>* vertice1Map;
	unordered_map<Vertice, int>::iterator vertice2Index;
	size_t highestVerticeIndexInEdge;

	if (edge.vertice1 > edge.vertice2) {
		highestVerticeIndexInEdge = edge.vertice1;
	} else {
		highestVerticeIndexInEdge = edge.vertice2;
	}

	if (highestVerticeIndexInEdge > this->highestVerticeIndex) {
		this->highestVerticeIndex = highestVerticeIndexInEdge;
	}


	vertice1Index = this->adjacencyListMap.find(edge.vertice1);
	if (vertice1Index == this->adjacencyListMap.end()) {
		vertice1Map = new unordered_map<size_t, int>();
		this->adjacencyListMap[edge.vertice1] = vertice1Map;
	} else {
		vertice1Map = vertice1Index->second;
	}

	vertice2Index = vertice1Map->find(edge.vertice2);

	if (vertice2Index == vertice1Map->end()) {
		(*vertice1Map)[edge.vertice2] = weight;
	}
}

TrafficGraph* TrafficGraphBuilder::buildAsAdjacencyMatrix(void) {
	size_t matrixDimension = this->highestVerticeIndex+1;
	size_t matrixTotalSize = matrixDimension*matrixDimension;
	Vertice i, j, aux;
	int* adjacencyMatrix = new int[matrixTotalSize];

	for (i = 0; i < matrixTotalSize; i++) {
		adjacencyMatrix[i] = -1;
	}

	for (auto& it: this->adjacencyListMap) {
		auto itVerticeMap = it.second;
		i = it.first;
		for (auto& jt: *itVerticeMap) {
			j = jt.first;
			if (i > j) {
				aux = i;
				i = j;
				j = aux;
			}
			adjacencyMatrix[i*matrixDimension+j] = jt.second;
		}
	}
	return new AdjacencyMatrixTrafficGraph(adjacencyMatrix, matrixDimension, this->cycle);
}

TrafficGraph* TrafficGraphBuilder::buildAsAdjacencyList(void) {
	size_t numberOfVertices = this->highestVerticeIndex+1;
	auto adjacencyList = new unordered_map<Vertice, int>[numberOfVertices];
	Vertice i, j;

	for (auto& it: this->adjacencyListMap) {
		auto itVerticeMap = it.second;
		i = it.first;
		for (auto& jt: *itVerticeMap) {
			j = jt.first;
			adjacencyList[i][j] = jt.second;
			adjacencyList[j][i] = jt.second;
		}
	}

	return new AdjacencyListTrafficGraph(adjacencyList, numberOfVertices, this->cycle);
}

void TrafficGraphBuilder::withCycle (int cycle) {
	this->cycle = cycle;
}
