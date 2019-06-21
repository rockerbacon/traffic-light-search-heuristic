#pragma once

#include "traffic_graph.h"

namespace traffic {
	void constructRandomSolution (Graph& graph);
	void constructHeuristicSolution (Graph& graph, unsigned char numberOfTuplesToTestPerIteration=3);

	TimeUnit distance(const Graph& graph, const Solution& a, const Solution& b);
};
