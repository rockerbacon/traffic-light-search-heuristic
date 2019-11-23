#include "traffic_graph.h"

#include <vector>
#include <random>
#include <algorithm>
#include <fstream>

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

bool GraphBuilder::addEdge(const Graph::Edge& edge, Weight weight) {
	decltype(GraphBuilder::adjacencyListMap)::iterator vertex1Index;
	unordered_map<Vertex, Weight>* vertex1Map;
	unordered_map<Vertex, Weight>::iterator vertex2Index;
	Vertex highestVertexIndexInEdge;
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
		return false;
	}

	if (highestVertexIndexInEdge > this->highestVertexIndex) {
		this->highestVertexIndex = highestVertexIndexInEdge;
	}

	vertex1Index = this->adjacencyListMap.find(i);
	if (vertex1Index == this->adjacencyListMap.end()) {
		vertex1Map = new unordered_map<Vertex, Weight>();
		this->adjacencyListMap[i] = vertex1Map;
	} else {
		vertex1Map = vertex1Index->second;
	}

	vertex2Index = vertex1Map->find(j);

	if (vertex2Index == vertex1Map->end()) {
		(*vertex1Map)[j] = weight;
		return true;
	} else {
		return false;
	}

}

GraphBuilder::GraphBuilder(Vertex nVertices, Vertex minDegree, Vertex maxDegree, Weight minWeight, Weight maxWeight) : GraphBuilder()
{
	if(nVertices < 2)
	{
		throw invalid_argument("nVertices must be greater than 1");
	}

	if(maxDegree < (2 * (nVertices - 1))/nVertices)
	{
		throw invalid_argument("maxDegree cannot be less than 2*(nVertices-1)/nVertices");
	}

	if (minDegree > maxDegree) {
		throw invalid_argument("minDegree cannot be greater than maxDegree");
	} else if (minDegree < 1) {
		throw invalid_argument("minDegree cannot be smaller than 1");
	}

	vector<Vertex> verticesToConnect;
	vector<Vertex> connectedVertices;
	Vertex *degree;
	Vertex randomConnectedVertex, nextVertexToConnect;
	Vertex randomIndex;
	Weight weight;
	random_device seeder;
	mt19937 randomEngine(seeder());
	uniform_int_distribution<Vertex> vertexPicker;
	uniform_int_distribution<Weight> weightPicker(minWeight, maxWeight);
	uniform_int_distribution<Vertex> degreePicker;

	for (auto mapIterator = this->adjacencyListMap.begin(); mapIterator != this->adjacencyListMap.end(); mapIterator++) {
		delete(mapIterator->second);
	}

	verticesToConnect.reserve(nVertices);
	connectedVertices.reserve(nVertices);
	degree = new Vertex[nVertices];

	for(Vertex i = 0; i < nVertices; i++)
	{
		verticesToConnect.push_back(i);
	}

	shuffle(verticesToConnect.begin(), verticesToConnect.end(), randomEngine);

	connectedVertices.push_back(verticesToConnect.back());
	verticesToConnect.pop_back();
	degree[connectedVertices.back()] = 0;

	while(!verticesToConnect.empty())
	{
		vertexPicker = uniform_int_distribution<Vertex>(0, connectedVertices.size()-1);
		randomIndex = vertexPicker(randomEngine);
		randomConnectedVertex = connectedVertices[randomIndex];

		if(degree[randomConnectedVertex] < maxDegree)
		{
			nextVertexToConnect = verticesToConnect.back();
			verticesToConnect.pop_back();

			weight = weightPicker(randomEngine);

			this->addEdge({randomConnectedVertex, nextVertexToConnect}, weight);

			degree[randomConnectedVertex]++;
			degree[nextVertexToConnect] = 1;

			connectedVertices.push_back(nextVertexToConnect);
		}

	}

	vertexPicker = decltype(vertexPicker)(0, nVertices-1);
	for(Vertex i = 0; i < nVertices; i++)
	{
		while (degree[i] < minDegree)
		{
			randomConnectedVertex = vertexPicker(randomEngine);
			weight = weightPicker(randomEngine);

			if (degree[randomConnectedVertex] < maxDegree) {
				if (this->addEdge({i, randomConnectedVertex}, weight)) {
					degree[i]++;
					degree[randomConnectedVertex]++;
				}
			}

		}

		if (degree[i] < maxDegree) {
			degreePicker = uniform_int_distribution<Vertex>(0, maxDegree-degree[i]);
			Vertex increaseDegreeBy = degreePicker(randomEngine);

			while (increaseDegreeBy > 0) {
				randomConnectedVertex = vertexPicker(randomEngine);
				weight = weightPicker(randomEngine);

				if (degree[randomConnectedVertex] < maxDegree) {
					if (this->addEdge({i, randomConnectedVertex}, weight)) {
						degree[i]++;
						degree[randomConnectedVertex]++;
						increaseDegreeBy--;
					}
				}

			}
		}
	}

	delete [] degree;
}

AdjacencyMatrixGraph* GraphBuilder::buildAsAdjacencyMatrix(void) const {
	Vertex matrixDimension = this->highestVertexIndex+1;
	Vertex matrixDimensionX2minus1 = matrixDimension*2-1;
	Vertex matrixTotalSize = matrixDimension/2*matrixDimension + matrixDimension/2;
	Vertex i, j;
	Vertex index;
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
	Vertex adjacencyListDimension = this->highestVertexIndex+1;
	auto adjacencyList = new unordered_map<Vertex, Weight>[adjacencyListDimension];
	Vertex i, j;

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

void GraphBuilder::output_to_file (ofstream &file_stream) const {
	file_stream << this->cycle << ' ' << this->adjacencyListMap.size() << '\n';
	for (auto &it: this->adjacencyListMap) {
		file_stream << it.first << ' ' << it.second->size();
		for (auto &jt: *it.second) {
			file_stream << ' ' << jt.first << ' ' << jt.second;
		}
		file_stream << '\n';
	}
}

void GraphBuilder::read_from_file(std::ifstream &file_stream) {
	TimeUnit cycle, weight;
	Vertex numberOfVertices, numberOfNeighbours, vertex, neighbour;
	Vertex lines = 1;

	file_stream >> cycle >> numberOfVertices;
	lines++;
	if (file_stream.fail()) {
		throw std::invalid_argument("Could not cycle and number of vertices for graph");
	}
	this->withCycle(cycle);
	for (Vertex i = 0; i < numberOfVertices; i++) {
		file_stream >> vertex >> numberOfNeighbours;
		if (file_stream.fail()) {
			throw std::invalid_argument("Could not read vertex index and degree at line "+to_string(lines));
		}
		for (Vertex j = 0; j < numberOfNeighbours; j++) {
			file_stream >> neighbour >> weight;
			if (file_stream.fail()) {
				throw std::invalid_argument("Could not read "+to_string(j)+"th neighbour index and edge weight at line "+to_string(lines));
			}
			this->addEdge({vertex, neighbour}, weight);
		}
		lines++;
	}

}
