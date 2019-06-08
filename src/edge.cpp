#include "traffic_graph.h"

using namespace ufrrj;
bool TrafficGraph::Edge::operator== (const TrafficGraph::Edge& other) {
	return	this->vertice1 == other.vertice1 && this->vertice2 == other.vertice2 ||
			this->vertice1 == other.vertice2 && this->vertice2 == other.vertice1;
}
