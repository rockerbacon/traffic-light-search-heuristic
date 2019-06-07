#include "traffic_graph.h"
#include <iostream>

using namespace ufrrj;
using namespace std;

/*TRAFFIC GRAPH*/
int TrafficGraph::weight (const TrafficGraph::Edge& edge) {
	return 0;
}

AdjacencyMatrixTrafficGraph::AdjacencyMatrixTrafficGraph(int* adjacencyMatrix, size_t numberOfVertices) {
	this->adjacencyMatrix = adjacencyMatrix;
	this->numberOfVertices = numberOfVertices;
}

int AdjacencyMatrixTrafficGraph::weight (const TrafficGraph::Edge& edge) {
	return this->adjacencyMatrix[edge.vertice1*this->numberOfVertices + edge.vertice2];
}

size_t AdjacencyMatrixTrafficGraph::getNumberOfVertices (void) {
	return this->numberOfVertices;
}
/*TRAFFIC GRAPH*/

/*EDGE*/
bool TrafficGraph::Edge::operator== (const TrafficGraph::Edge& other) {
	return	this->vertice1 == other.vertice1 && this->vertice2 == other.vertice2 ||
			this->vertice1 == other.vertice2 && this->vertice2 == other.vertice1;
}
/*EDGE*/

/*TRAFFIC GRAPH BUILDER*/
TrafficGraphBuilder::TrafficGraphBuilder () {
	this->highestVerticeIndex = 0;
}

TrafficGraphBuilder::~TrafficGraphBuilder (void) {
	for (auto mapIterator = this->adjacencyListMap.begin(); mapIterator != this->adjacencyListMap.end(); mapIterator++) {
		delete(mapIterator->second);
	}
}

void TrafficGraphBuilder::addEdgeBetween(const TrafficGraph::Edge& edge, int weight) {
	decltype(TrafficGraphBuilder::adjacencyListMap)::iterator vertice1Index;
	unordered_map<size_t, int>* vertice1Map;
	unordered_map<size_t, int>::iterator vertice2Index;
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
	size_t i,j;
	int* adjacencyMatrix = new int[matrixTotalSize];
	for (auto& it: this->adjacencyListMap) {
		auto itVerticeMap = it.second;
		i = it.first;
		for (auto& jt: *itVerticeMap) {
			j = jt.first;
			adjacencyMatrix[i*matrixDimension+j] = jt.second;
		}
	}
	return new AdjacencyMatrixTrafficGraph(adjacencyMatrix, matrixDimension);
}

TrafficGraph* TrafficGraphBuilder::buildAsAdjacencyList(void) {
	return NULL;
}
/*TRAFFIC GRAPH BUILDER*/
