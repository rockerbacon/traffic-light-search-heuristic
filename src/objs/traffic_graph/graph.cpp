#include "traffic_graph.h"

using namespace traffic;
Graph::Graph(size_t numberOfVertices, TimeUnit cycle) {
	this->numberOfVertices = numberOfVertices;
	this->cycle = cycle;
}

Graph::~Graph (void) {}

size_t Graph::getNumberOfVertices (void) const {
	return this->numberOfVertices;
}

TimeUnit Graph::penalty (Vertex vertex1, Vertex vertex2, const Solution& solution, Weight edgeWeight) const {
	TimeUnit timingVertex1 = solution.getTiming(vertex1);
	TimeUnit timingVertex2 = solution.getTiming(vertex2);
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

TimeUnit Graph::vertexPenalty (Vertex vertex, const Solution& solution) const {
	TimeUnit totalPenalty = 0;
	for (auto neighbor : this->neighborsOf(vertex)) {
		totalPenalty += this->penalty(vertex, neighbor.first, solution, neighbor.second);
		totalPenalty += this->penalty(neighbor.first, vertex, solution, neighbor.second);
	}
	return totalPenalty;
}

TimeUnit Graph::vertexPenaltyOnewayOnly (Vertex vertex, const Solution& solution) const {
	TimeUnit totalPenalty = 0;
	for (auto neighbor : this->neighborsOf(vertex)) {
		totalPenalty += this->penalty(vertex, neighbor.first, solution, neighbor.second);
	}
	return totalPenalty;
}

TimeUnit Graph::totalPenalty (const Solution& solution) const {
	TimeUnit totalPenalty = 0;
	for (Vertex v = 0; v < this->numberOfVertices; v++) {
		totalPenalty += this->vertexPenaltyOnewayOnly(v, solution);
	}
	return totalPenalty;
}

TimeUnit Graph::lowerBound (void) const {
	TimeUnit lowerBound = 0;
	TimeUnit weightTimes2;
	Edge edge;

	for (edge.vertex1 = 0; edge.vertex1 < this->numberOfVertices-1; edge.vertex1++) {
		for (edge.vertex2 = edge.vertex1+1; edge.vertex2 < this->numberOfVertices; edge.vertex2++) {
			weightTimes2 = this->weight(edge);
			if (weightTimes2 != -1) {
				weightTimes2 = weightTimes2*2%this->getCycle();
				if (weightTimes2 > this->getCycle() - weightTimes2) {
					lowerBound += this->getCycle() - weightTimes2;
				} else {
					lowerBound += weightTimes2;
				}
			}
		}
	}

	return lowerBound;
}
