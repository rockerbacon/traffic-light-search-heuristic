#include <assertions-test/test.h>
#include <traffic_graph/traffic_graph.h>
#include <heuristic/heuristic.h>
#include "mock_graph.h"

#define NUMBER_OF_THREADS 4

using namespace traffic;
using namespace std;
using namespace heuristic;

begin_tests {
	test_suite("when performing parallel scatter search") {
		test_case("should throw error when total size of the population is not even") {
			MockGraph graph;
			bool exception_raised = false;
			try {
				heuristic::parallel::scatterSearch(graph, 3, 4, 1, stop_function_factory::numberOfIterations(1), combination_method_factory::breadthFirstSearch(0.2), NUMBER_OF_THREADS);
			} catch(invalid_argument &e) {
				exception_raised = true;
			}
			assert(exception_raised, ==, true);
		};

		test_case("scatter search solution should have at least one timing different from 0") {
			MockGraph graph;
			size_t elitePopulationSize = NUMBER_OF_THREADS;
			size_t diversePopulationSize = NUMBER_OF_THREADS*3;
			size_t localSearchIterations = 10;
			double mutationProbability = 0.2;

			auto stopFunction = stop_function_factory::numberOfIterations(3);
			auto combinationMethod = combination_method_factory::breadthFirstSearch(mutationProbability);

			auto searchedSolution = heuristic::parallel::scatterSearch(graph, elitePopulationSize, diversePopulationSize, localSearchIterations, stopFunction, combinationMethod, NUMBER_OF_THREADS);

			auto has_non_zero_timing = false;
			for (Vertex v = 0; v < searchedSolution.getNumberOfVertices() && !has_non_zero_timing; v++) {
				if (searchedSolution.getTiming(v) != 0) {
					has_non_zero_timing = true;
				}
			}

			assert(has_non_zero_timing, ==, true);
		};

		test_case("scatter search solution should be better than solution with 0 timings") {
			MockGraph graph;
			Solution zeroTimingSolution(graph.getNumberOfVertices());

			size_t elitePopulationSize = NUMBER_OF_THREADS;
			size_t diversePopulationSize = NUMBER_OF_THREADS*3;
			size_t localSearchIterations = 10;
			double mutationProbability = 0.2;

			auto stopFunction = stop_function_factory::numberOfIterations(3);
			auto combinationMethod = combination_method_factory::breadthFirstSearch(mutationProbability);

			auto searchedSolution = heuristic::parallel::scatterSearch(graph, elitePopulationSize, diversePopulationSize, localSearchIterations, stopFunction, combinationMethod, NUMBER_OF_THREADS);

			assert(graph.totalPenalty(searchedSolution), <, graph.totalPenalty(zeroTimingSolution));
		};

		test_case("scatter search solution should have all timings in the interval [0, cycle)") {
			MockGraph graph;
			size_t elitePopulationSize = NUMBER_OF_THREADS;
			size_t diversePopulationSize = NUMBER_OF_THREADS*3;
			size_t localSearchIterations = 10;
			double mutationProbability = 0.2;

			auto stopFunction = stop_function_factory::numberOfIterations(3);
			auto combinationMethod = combination_method_factory::breadthFirstSearch(mutationProbability);

			auto searchedSolution = heuristic::parallel::scatterSearch(graph, elitePopulationSize, diversePopulationSize, localSearchIterations, stopFunction, combinationMethod, NUMBER_OF_THREADS);

			for (Vertex v = 0; v < searchedSolution.getNumberOfVertices(); v++) {
				auto timing = searchedSolution.getTiming(v);
				assert(timing, >=, 0);
				assert(timing, <, graph.getCycle());
			}
		};
	}
} end_tests;
