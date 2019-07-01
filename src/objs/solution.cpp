#include "traffic_graph.h"

using namespace traffic;
using namespace std;

Solution::Solution (void) {
	this->vertexTimings = nullptr;
	this->numberOfVertices = 0;
}

Solution::Solution(size_t numberOfVertices) {
	this->vertexTimings = new TimeUnit[numberOfVertices];
	this->numberOfVertices = numberOfVertices;
	for (size_t i = 0; i < numberOfVertices; i++) {
		this->vertexTimings[i] = 0;
	}
}

Solution::Solution (const Solution& other) {
	this->vertexTimings = new TimeUnit[other.getNumberOfVertices()];
	this->numberOfVertices = other.getNumberOfVertices();
	for (Vertex v = 0; v < other.getNumberOfVertices(); v++) {
		this->setTiming(v, other.getTiming(v));
	}
}

Solution::Solution (Solution&& other) {
	this->vertexTimings = nullptr;
	swap(*this, other);
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

size_t Solution::getNumberOfVertices (void) const {
	return this->numberOfVertices;
}

Solution& Solution::operator= (Solution other) {
	swap(*this, other);
	return *this;
}
