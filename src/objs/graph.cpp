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
