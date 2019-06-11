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

bool GraphBuilder::generateRandomGraph(size_t nVertices, unsigned maxDegree, int minWeight, int maxWeight)
{
	if(nVertices < 2)
	{
		return false;
	}

	unsigned minEdges = nVertices - 1;

	//https://math.stackexchange.com/questions/237103/proof-related-to-minimum-and-maximum-degree-of-vertices-of-an-undirected-graph
	if(maxDegree < (2 * minEdges)/nVertices)
	{
		return false;
	}

	if(maxDegree > nVertices - 1)
	{
		return false;
	}

	srand(time(NULL));
	unsigned dst[nVertices], dstEnd = nVertices;
	unsigned src[nVertices], srcEnd = 0;
	unsigned spareDegrees[nVertices];
	unsigned a, b, weight;
	size_t r;

	for(size_t i = 0; i < nVertices; i++)
	{
		dst[i] = i;
		spareDegrees[i] = maxDegree;
	}

	random_shuffle(dst, dst + nVertices);

	src[srcEnd++] = dst[--dstEnd];

	while(dstEnd != 0)
	{
		r = rand() % srcEnd;
		a = src[r];

		if(!spareDegrees[a])
		{
			continue;
		}

		b = dst[--dstEnd];

		spareDegrees[a]--;
		spareDegrees[b]--;

		weight = rand() % (maxWeight - minWeight + 1) + minWeight;

		this->addEdge({a, b}, weight);
		this->addEdge({b, a}, weight);

		src[srcEnd++] = b;
	}

	for(size_t i = 0; i < nVertices; i++)
	{
		if(spareDegrees[dst[i]])
		{
			r = rand() % (spareDegrees[dst[i]] + 1);

			for(size_t j = i + 1; j < nVertices && r > 0; j++, r--)
			{
				if(spareDegrees[dst[j]])
				{
					spareDegrees[dst[i]]--;
					spareDegrees[dst[j]]--;
				}
			}
		}
	}

	return true;
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
	size_t adjacencyListDimension = this->highestVerticeIndex;
	auto adjacencyList = new unordered_map<Vertice, Weight>[adjacencyListDimension];
	Vertice i, j, aux;

	for (auto& it: this->adjacencyListMap) {
		auto itVerticeMap = it.second;
		i = it.first;
		for (auto& jt: *itVerticeMap) {
			j = jt.first;
			adjacencyList[i][j] = jt.second;
		}
	}

	return new AdjacencyListGraph(adjacencyList, adjacencyListDimension+1, this->cycle);
}

void GraphBuilder::withCycle (TimeUnit cycle) {
	this->cycle = cycle;
}
