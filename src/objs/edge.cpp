#include "traffic_graph.h"

using namespace traffic;
bool Graph::Edge::operator== (const Graph::Edge& other) const {
	return	this->vertex1 == other.vertex1 && this->vertex2 == other.vertex2 ||
			this->vertex1 == other.vertex2 && this->vertex2 == other.vertex1;
}
