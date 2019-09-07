#pragma once

#include "traffic_graph/traffic_graph.h"
#include <unordered_set>
#include <functional>
#include <chrono>

namespace heuristic {
	traffic::Solution constructRandomSolution (const traffic::Graph& graph);
	traffic::Solution constructHeuristicSolution (const traffic::Graph& graph, unsigned char numberOfTuplesToTestPerIteration=3);

	traffic::TimeUnit distance(const traffic::Graph& graph, const traffic::Solution& a, const traffic::Solution& b);

	struct Metrics {
		unsigned numberOfIterations;
		unsigned numberOfIterationsWithoutImprovement;
		std::chrono::high_resolution_clock::time_point executionBegin;
	};

	typedef std::function<bool(const Metrics&)> StopFunction;

	namespace stop_function_factory {

		StopFunction numberOfIterations (unsigned numberOfIterationsToStop);
		StopFunction numberOfIterationsWithoutImprovement (unsigned numberOfIterationsToStop);

		template<typename Rep, typename Period=std::ratio<1>>
		StopFunction executionTime(const std::chrono::duration<Rep, Period>& time) {
			std::chrono::high_resolution_clock::duration highResolutionTime = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(time);

			return [=](const Metrics &metrics) -> bool {
				return std::chrono::high_resolution_clock::now() - metrics.executionBegin < highResolutionTime;
			};
		}

	};

	typedef std::function<traffic::Solution(const traffic::Graph&, const traffic::Solution*, const traffic::Solution*c)> CombinationMethod;

	namespace combination_method_factory{

		CombinationMethod breadthFirstSearch(void);
		CombinationMethod crossover(double mutationProbability);

	}

	traffic::Solution localSearchHeuristic(const traffic::Graph& graph, const traffic::Solution& initialSolution, const std::function<bool(const Metrics&)>& stopCriteriaNotMet);
	traffic::Solution scatterSearch (const traffic::Graph& graph, size_t elitePopulationSize, size_t diversePopulationSize, size_t localSearchIterations, const StopFunction &stopFunction, const CombinationMethod &combinationMethod);
	traffic::Solution geneticAlgorithm(const traffic::Graph& graph, size_t populationSize, const StopFunction &stopFunction, const CombinationMethod &combinationMethod);

	namespace parallel {
		traffic::Solution scatterSearch (const traffic::Graph& graph, size_t elitePopulationSize, size_t diversePopulationSize, size_t localSearchIterations, const StopFunction &stopFunction, const CombinationMethod &combinationMethod, unsigned numberOfThreads);
	}
};
