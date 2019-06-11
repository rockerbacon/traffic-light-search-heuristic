#include "traffic_graph.h"

using namespace traffic;
Graph::Graph(size_t numberOfVertices, TimeUnit cycle) {
	this->solution = new Solution(numberOfVertices);
	this->numberOfVertices = numberOfVertices;
	this->cycle = cycle;
}

Graph::~Graph(void) {
	delete this->solution;
}

size_t Graph::getNumberOfVertices (void) const {
	return this->numberOfVertices;
}

void Graph::setTiming (Vertex vertex, TimeUnit timing) {
	this->solution->setTiming(vertex, timing);
}

TimeUnit Graph::getTiming (Vertex vertex) const {
	this->solution->getTiming(vertex);
}

TimeUnit Graph::penalty (Vertex vertex1, Vertex vertex2, Weight edgeWeight) const {
	TimeUnit timingVertex1 = this->solution->getTiming(vertex1);
	TimeUnit timingVertex2 = this->solution->getTiming(vertex2);
	Weight weight = (edgeWeight == -1) ? this->weight({vertex1, vertex2}) : edgeWeight;
	TimeUnit cuv, penalty;

	if (weight == -1) {
		return 0;
	} else {
		cuv = abs(timingVertex2 - weight - timingVertex1)%this->cycle;
	}

	if (cuv < this->cycle - cuv) {
		penalty = cuv;
	} else {
		penalty = this->cycle - cuv;
	}

	return penalty;
}

TimeUnit Graph::getCycle (void) const {
	return this->cycle;
}

TimeUnit Graph::vertexPenalty (Vertex vertex) const {
	TimeUnit totalPenalty = 0;
	for (auto neighbor : this->neighborsOf(vertex)) {
		totalPenalty += this->penalty(vertex, neighbor.first, neighbor.second);
		totalPenalty += this->penalty(neighbor.first, vertex, neighbor.second);
	}
	return totalPenalty;
}

TimeUnit Graph::vertexPenaltyOnewayOnly (Vertex vertex) const {
	TimeUnit totalPenalty = 0;
	for (auto neighbor : this->neighborsOf(vertex)) {
		totalPenalty += this->penalty(vertex, neighbor.first, neighbor.second);
	}
	return totalPenalty;
}
