#include "traffic_graph.h"

using namespace ufrrj;
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
