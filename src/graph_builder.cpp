#include "traffic_graph.h"

using namespace traffic;
using namespace std;
GraphBuilder::GraphBuilder () {
	this->highestVerticeIndex = 0;
}

GraphBuilder::~GraphBuilder (void) {
	for (auto mapIterator = this->adjacencyListMap.begin(); mapIterator != this->adjacencyListMap.end(); mapIterator++) {
		delete(mapIterator->second);
	}
}

void GraphBuilder::addEdge(const Graph::Edge& edge, Weight weight) {
	decltype(GraphBuilder::adjacencyListMap)::iterator vertice1Index;
	unordered_map<Vertice, Weight>* vertice1Map;
	unordered_map<Vertice, Weight>::iterator vertice2Index;
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
		vertice1Map = new unordered_map<size_t, Weight>();
		this->adjacencyListMap[edge.vertice1] = vertice1Map;
	} else {
		vertice1Map = vertice1Index->second;
	}

	vertice2Index = vertice1Map->find(edge.vertice2);

	if (vertice2Index == vertice1Map->end()) {
		(*vertice1Map)[edge.vertice2] = weight;
	}
}

Graph* GraphBuilder::buildAsAdjacencyMatrix(void) {
	size_t matrixDimension = this->highestVerticeIndex+1;
	size_t matrixTotalSize = matrixDimension*matrixDimension;
	Vertice i, j, aux;
	Weight* adjacencyMatrix = new Weight[matrixTotalSize];

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
	return new AdjacencyMatrixGraph(adjacencyMatrix, matrixDimension, this->cycle);
}

Graph* GraphBuilder::buildAsAdjacencyList(void) {
	size_t adjacencyListDimension = this->highestVerticeIndex;
	auto adjacencyList = new unordered_map<Vertice, Weight>[adjacencyListDimension];
	Vertice i, j, aux;

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
			adjacencyList[i][j] = jt.second;
		}
	}

	return new AdjacencyListGraph(adjacencyList, adjacencyListDimension+1, this->cycle);
}

void GraphBuilder::withCycle (TimeUnit cycle) {
	this->cycle = cycle;
}
