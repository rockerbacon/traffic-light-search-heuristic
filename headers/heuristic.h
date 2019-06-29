#pragma once

#include "traffic_graph.h"
#include <unordered_set>
#include <functional>

namespace traffic {
	Solution constructRandomSolution (const Graph& graph);
	Solution constructHeuristicSolution (const Graph& graph, unsigned char numberOfTuplesToTestPerIteration=3);

	TimeUnit distance(const Graph& graph, const Solution& a, const Solution& b);

	struct HeuristicMetrics {
		unsigned numberOfIterations;
		unsigned numberOfIterationsWithoutImprovement;
	};

	namespace stop_criteria {
		std::function<bool(const HeuristicMetrics&)> numberOfIterations (unsigned numberOfIterationsToStop);
		std::function<bool(const HeuristicMetrics&)> numberOfIterationsWithoutImprovement (unsigned numberOfIterationsToStop);
	};

	Solution localSearchHeuristic(const Graph& graph, const Solution& initialSolution, const std::function<bool(const HeuristicMetrics&)>& stopCriteriaNotMet);
	Solution populationalHeuristic(const Graph& graph, size_t elitePopulationSize, size_t diversePopulationSize, const std::function<bool(const HeuristicMetrics&)>& stopCriteriaNotMet);
};
