#include "traffic_graph.h"

using namespace traffic;
using namespace std;
Solution::Solution(size_t numberOfVertices) {
	this->vertexTimings = new TimeUnit[numberOfVertices];
	for (size_t i = 0; i < numberOfVertices; i++) {
		this->vertexTimings[i] = 0;
	}
}

Solution::~Solution (void) {
	delete [] this->vertexTimings;
}

void Solution::setTiming (Vertex vertex, TimeUnit timing) {
	this->vertexTimings[vertex] = timing;
}

TimeUnit Solution::getTiming (Vertex vertex) const {
	return this->vertexTimings[vertex];
}
