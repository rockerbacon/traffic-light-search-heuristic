#pragma once

#include "traffic_graph.h"
#include <unordered_set>
#include <functional>

namespace traffic {
	Solution constructRandomSolution (const Graph& graph);
	Solution constructHeuristicSolution (const Graph& graph, unsigned char numberOfTuplesToTestPerIteration=3);

	TimeUnit distance(const Graph& graph, const Solution& a, const Solution& b);

	struct LocalSearchMetrics {
		unsigned numberOfIterations;
		unsigned numberOfIterationsWithoutImprovement;
	};

	namespace stop_criteria {
		std::function<bool(const LocalSearchMetrics&)> numberOfIterations (unsigned numberOfIterationsToStop);
		std::function<bool(const LocalSearchMetrics&)> numberOfIterationsWithoutImprovement (unsigned numberOfIterationsToStop);
	};

	Solution localSearchHeuristic(const Graph& graph, const Solution& initialSolution, const std::function<bool(const LocalSearchMetrics&)>& stopCriteriaNotMet);
	
	Solution combine(const Graph& graph, const Solution& s1, const Solution& s2);
	Solution crossover(const Graph& graph, const Solution& s1, const Solution& s2);
};
