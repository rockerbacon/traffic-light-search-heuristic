#include "traffic_graph.h"
#include <iostream>
#include <math.h>

using namespace ufrrj;
using namespace std;

/*TRAFFIC GRAPH*/
Solution::Solution(size_t numberOfVertices) {
	this->verticeTimings = new int[numberOfVertices];
	for (int i = 0; i < numberOfVertices; i++) {
		this->verticeTimings[i] = 0;
	}
}

Solution::~Solution (void) {
	delete this->verticeTimings;
}

void Solution::setTiming (Vertice vertice, int timing) {
	this->verticeTimings[vertice] = timing;
}

int Solution::getTiming (Vertice vertice) {
	return this->verticeTimings[vertice];
}

AdjacencyMatrixTrafficGraph::AdjacencyMatrixTrafficGraph(int* adjacencyMatrix, size_t numberOfVertices, int cycle) {
	this->adjacencyMatrix = adjacencyMatrix;
	this->numberOfVertices = numberOfVertices;
	this->solution = new Solution(numberOfVertices);
	this->cycle = cycle;
}

AdjacencyMatrixTrafficGraph::~AdjacencyMatrixTrafficGraph (void) {
	delete this->solution;
	delete this->adjacencyMatrix;
}

int AdjacencyMatrixTrafficGraph::weight (const TrafficGraph::Edge& edge) const {
	size_t i, j;
	if (edge.vertice1 > edge.vertice2) {
		i = edge.vertice2;
		j = edge.vertice1;
	} else {
		i = edge.vertice1;
		j = edge.vertice2;
	}
	return this->adjacencyMatrix[i*this->numberOfVertices + j];
}

size_t AdjacencyMatrixTrafficGraph::getNumberOfVertices (void) const {
	return this->numberOfVertices;
}

void AdjacencyMatrixTrafficGraph::setTiming (Vertice vertice, int timing) {
	this->solution->setTiming(vertice, timing);
}

int AdjacencyMatrixTrafficGraph::getTiming (Vertice vertice) const {
	this->solution->getTiming(vertice);
}

int AdjacencyMatrixTrafficGraph::penalty (Vertice vertice1, Vertice vertice2) const {
	int timingVertice1 = this->solution->getTiming(vertice1);
	int timingVertice2 = this->solution->getTiming(vertice2);
	int weight = this->weight({vertice1, vertice2});
	int cuv, penalty;

	if (weight == -1) {
		return 0;
	} else {
		cuv = abs(timingVertice2 - weight - timingVertice1)%this->cycle;
	}

	if (cuv < this->cycle - cuv) {
		penalty = cuv;
	} else {
		penalty = this->cycle - cuv;
	}

	return penalty;
}

int AdjacencyMatrixTrafficGraph::getCycle (void) const {
	return this->cycle;
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

void TrafficGraphBuilder::addEdge(const TrafficGraph::Edge& edge, int weight) {
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

	for (i = 0; i < matrixTotalSize; i++) {
		adjacencyMatrix[i] = -1;
	}

	for (auto& it: this->adjacencyListMap) {
		auto itVerticeMap = it.second;
		i = it.first;
		for (auto& jt: *itVerticeMap) {
			j = jt.first;
			adjacencyMatrix[i*matrixDimension+j] = jt.second;
		}
	}
	return new AdjacencyMatrixTrafficGraph(adjacencyMatrix, matrixDimension, this->cycle);
}

TrafficGraph* TrafficGraphBuilder::buildAsAdjacencyList(void) {
	return NULL;
}

void TrafficGraphBuilder::withCycle (int cycle) {
	this->cycle = cycle;
}

/*TRAFFIC GRAPH BUILDER*/
