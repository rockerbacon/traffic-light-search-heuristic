#include "traffic_graph.h"

using namespace ufrrj;
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
