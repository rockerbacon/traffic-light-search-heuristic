#include "traffic_graph.h"

using namespace traffic;
using namespace std;
GraphBuilder::GraphBuilder () {
	this->highestVertexIndex = 0;
}

GraphBuilder::~GraphBuilder (void) {
	for (auto mapIterator = this->adjacencyListMap.begin(); mapIterator != this->adjacencyListMap.end(); mapIterator++) {
		delete(mapIterator->second);
	}
}

void GraphBuilder::addEdge(const Graph::Edge& edge, Weight weight) {
	decltype(GraphBuilder::adjacencyListMap)::iterator vertex1Index;
	unordered_map<Vertex, Weight>* vertex1Map;
	unordered_map<Vertex, Weight>::iterator vertex2Index;
	size_t highestVertexIndexInEdge;
	Vertex i, j;

	if (edge.vertex1 > edge.vertex2) {
		highestVertexIndexInEdge = edge.vertex1;
		i = edge.vertex2;
		j = edge.vertex1;
	} else if (edge.vertex1 < edge.vertex2){
		highestVertexIndexInEdge = edge.vertex2;
		i = edge.vertex1;
		j = edge.vertex2;
	} else {
		return;
	}

	if (highestVertexIndexInEdge > this->highestVertexIndex) {
		this->highestVertexIndex = highestVertexIndexInEdge;
	}


	vertex1Index = this->adjacencyListMap.find(i);
	if (vertex1Index == this->adjacencyListMap.end()) {
		vertex1Map = new unordered_map<size_t, Weight>();
		this->adjacencyListMap[i] = vertex1Map;
	} else {
		vertex1Map = vertex1Index->second;
	}

	vertex2Index = vertex1Map->find(j);

	if (vertex2Index == vertex1Map->end()) {
		(*vertex1Map)[j] = weight;
	}
}

AdjacencyMatrixGraph* GraphBuilder::buildAsAdjacencyMatrix(void) const {
	size_t matrixDimension = this->highestVertexIndex+1;
	size_t matrixDimensionX2minus1 = matrixDimension*2-1;
	size_t matrixTotalSize = matrixDimension/2*matrixDimension + matrixDimension/2;
	Vertex i, j;
	size_t index;
	Weight* adjacencyMatrix = new Weight[matrixTotalSize];

	for (i = 0; i < matrixTotalSize; i++) {
		adjacencyMatrix[i] = -1;
	}

	for (auto& it: this->adjacencyListMap) {
		auto itVertexMap = it.second;
		i = it.first;
		for (auto& jt: *itVertexMap) {
			j = jt.first;
			index = j + i*(matrixDimensionX2minus1-i)/2;
			adjacencyMatrix[index] = jt.second;
		}
	}
	return new AdjacencyMatrixGraph(adjacencyMatrix, matrixDimension, this->cycle);
}

AdjacencyListGraph* GraphBuilder::buildAsAdjacencyList(void) const {
	size_t adjacencyListDimension = this->highestVertexIndex+1;
	auto adjacencyList = new unordered_map<Vertex, Weight>[adjacencyListDimension];
	Vertex i, j, aux;

	for (auto& it: this->adjacencyListMap) {
		auto itVertexMap = it.second;
		i = it.first;
		for (auto& jt: *itVertexMap) {
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
