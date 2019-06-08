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

void Graph::setTiming (Vertice vertice, TimeUnit timing) {
	this->solution->setTiming(vertice, timing);
}

TimeUnit Graph::getTiming (Vertice vertice) const {
	this->solution->getTiming(vertice);
}

TimeUnit Graph::penalty (Vertice vertice1, Vertice vertice2) const {
	TimeUnit timingVertice1 = this->solution->getTiming(vertice1);
	TimeUnit timingVertice2 = this->solution->getTiming(vertice2);
	Weight weight = this->weight({vertice1, vertice2});
	TimeUnit cuv, penalty;

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

TimeUnit Graph::getCycle (void) const {
	return this->cycle;
}