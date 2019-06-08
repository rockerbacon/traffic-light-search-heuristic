#include "traffic_graph.h"

using namespace traffic;
Solution::Solution(size_t numberOfVertices) {
	this->verticeTimings = new TimeUnit[numberOfVertices];
	for (size_t i = 0; i < numberOfVertices; i++) {
		this->verticeTimings[i] = 0;
	}
}

Solution::~Solution (void) {
	delete this->verticeTimings;
}

void Solution::setTiming (Vertice vertice, TimeUnit timing) {
	this->verticeTimings[vertice] = timing;
}

TimeUnit Solution::getTiming (Vertice vertice) {
	return this->verticeTimings[vertice];
}
