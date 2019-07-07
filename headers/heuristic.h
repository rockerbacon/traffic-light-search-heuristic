#pragma once

#include "traffic_graph.h"
#include <unordered_set>
#include <functional>
#include <chrono>

namespace traffic {
	Solution constructRandomSolution (const Graph& graph);
	Solution constructHeuristicSolution (const Graph& graph, unsigned char numberOfTuplesToTestPerIteration=3);

	TimeUnit distance(const Graph& graph, const Solution& a, const Solution& b);

	struct HeuristicMetrics {
		unsigned numberOfIterations;
		unsigned numberOfIterationsWithoutImprovement;
		std::chrono::high_resolution_clock::time_point executionBegin;
	};

	namespace stop_criteria {
		std::function<bool(const HeuristicMetrics&)> numberOfIterations (unsigned numberOfIterationsToStop);
		std::function<bool(const HeuristicMetrics&)> numberOfIterationsWithoutImprovement (unsigned numberOfIterationsToStop);
		template<typename Rep, typename Period=std::ratio<1>>
		std::function<bool(const HeuristicMetrics&)> executionTime(const std::chrono::duration<Rep, Period>& time) {
			std::chrono::high_resolution_clock::duration highResolutionTime = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(time);

			return [=](const HeuristicMetrics &metrics) -> bool {
				return std::chrono::high_resolution_clock::now() - metrics.executionBegin < highResolutionTime;
			};
		}
	};

	Solution localSearchHeuristic(const Graph& graph, const Solution& initialSolution, const std::function<bool(const HeuristicMetrics&)>& stopCriteriaNotMet);
	Solution populationalHeuristic(const Graph& graph, size_t elitePopulationSize, size_t diversePopulationSize, size_t localSearchIterations, const std::function<bool(const HeuristicMetrics&)>& stopCriteriaNotMet, Solution (*combineMethodFunction)(const Graph&, const Solution*, const Solution*, int, double));
	Solution combineByBfs(const Graph& graph, const Solution *s1, const Solution *s2);
	Solution combineByBfs_aux(const Graph& graph, const Solution *s1, const Solution *s2, int pRange, double mutationProb);//apenas chama combineByBfs
	Solution crossover(const Graph& graph, const Solution *a, const Solution *b, int pRange, double mutationProb);
	Solution geneticAlgorithm(const Graph& graph, size_t populationSize, double mutationProb, const std::function<bool(const HeuristicMetrics&)>& stopCriteriaNotMet, Solution (*combinationFunction)(const Graph&, const Solution*, const Solution*, int, double));

};
