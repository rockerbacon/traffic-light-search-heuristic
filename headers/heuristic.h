#pragma once

#include "traffic_graph.h"
#include <unordered_set>

namespace traffic {
	Solution constructRandomSolution (const Graph& graph);
	Solution constructHeuristicSolution (const Graph& graph, unsigned char numberOfTuplesToTestPerIteration=3);

	TimeUnit distance(const Graph& graph, const Solution& a, const Solution& b);

	Solution localSearchHeuristic(const Graph& graph, const Solution& initialSolution, unsigned numberOfPerturbations, size_t perturbationHistorySize);
};
