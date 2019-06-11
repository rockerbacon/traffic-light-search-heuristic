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
	Vertice i, j;

	if (edge.vertice1 > edge.vertice2) {
		highestVerticeIndexInEdge = edge.vertice1;
		i = edge.vertice2;
		j = edge.vertice1;
	} else if (edge.vertice1 < edge.vertice2){
		highestVerticeIndexInEdge = edge.vertice2;
		i = edge.vertice1;
		j = edge.vertice2;
	} else {
		return;
	}

	if (highestVerticeIndexInEdge > this->highestVerticeIndex) {
		this->highestVerticeIndex = highestVerticeIndexInEdge;
	}


	vertice1Index = this->adjacencyListMap.find(i);
	if (vertice1Index == this->adjacencyListMap.end()) {
		vertice1Map = new unordered_map<size_t, Weight>();
		this->adjacencyListMap[i] = vertice1Map;
	} else {
		vertice1Map = vertice1Index->second;
	}

	vertice2Index = vertice1Map->find(j);

	if (vertice2Index == vertice1Map->end()) {
		(*vertice1Map)[j] = weight;
	}
}

Graph* GraphBuilder::buildAsAdjacencyMatrix(void) {
	size_t matrixDimension = this->highestVerticeIndex+1;
	size_t matrixDimensionX2minus1 = matrixDimension*2-1;
	size_t matrixTotalSize = matrixDimension/2*matrixDimension + matrixDimension/2;
	Vertice i, j;
	size_t index;
	Weight* adjacencyMatrix = new Weight[matrixTotalSize];

	for (i = 0; i < matrixTotalSize; i++) {
		adjacencyMatrix[i] = -1;
	}

	for (auto& it: this->adjacencyListMap) {
		auto itVerticeMap = it.second;
		i = it.first;
		for (auto& jt: *itVerticeMap) {
			j = jt.first;
			index = j + i*(matrixDimensionX2minus1-i)/2;
			adjacencyMatrix[index] = jt.second;
		}
	}
	return new AdjacencyMatrixGraph(adjacencyMatrix, matrixDimension, this->cycle);
}

Graph* GraphBuilder::buildAsAdjacencyList(void) {
	size_t adjacencyListDimension = this->highestVerticeIndex+1;
	auto adjacencyList = new unordered_map<Vertice, Weight>[adjacencyListDimension];
	Vertice i, j, aux;

	for (auto& it: this->adjacencyListMap) {
		auto itVerticeMap = it.second;
		i = it.first;
		for (auto& jt: *itVerticeMap) {
			j = jt.first;
			adjacencyList[i][j] = jt.second;
			adjacencyList[j][i] = jt.second;
		}
	}

	return new AdjacencyListGraph(adjacencyList, adjacencyListDimension, this->cycle);
}

void GraphBuilder::withCycle (TimeUnit cycle) {
	this->cycle = cycle;
}
