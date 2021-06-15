#include <assertions-test/test.h>
#include <traffic_graph/traffic_graph.h>
#include <heuristic/heuristic.h>
#include "mock_graph.h"

using namespace traffic;
using namespace std;
using namespace heuristic;

tests {
	test_suite("when diversifying populations") {
		test_case("distance between two equal solutions is 0") {
			MockGraph graph;
			Solution solution1(graph.getNumberOfVertices());
			Solution solution2(graph.getNumberOfVertices());
			for (Vertex i = 0; i < numberOfTestVertices; i++) {
				solution1[i] = i;
				solution2[i] = i;
			}
			assert(distance(graph, solution1, solution2), ==, 0);
		};

		test_case("distance between two solutions should be correctly calculated") {
			MockGraph graph;
			Solution solution1(graph.getNumberOfVertices(), 0);
			Solution solution2(graph.getNumberOfVertices(), 0);
			//3
			solution1[0] = 18;
			solution2[0] = 1;
			//4
			solution1[1] = 4;
			solution2[1] = 8;
			//10
			solution1[2] = 10;
			solution2[2] = 0;

			auto calculatedDistance = distance(graph, solution1, solution2);
			assert(calculatedDistance, ==, 3+4+10);
		};
	}

	test_suite("when combining individuals") {
		test_case("combining two equal solutions using breadth first search without mutation probability should not alter the solution") {
			MockGraph graph;
			Solution initialSolution(graph.getNumberOfVertices());

			for (Vertex v = 0; v < initialSolution.size(); v++) {
				initialSolution[v] = v*2;
			}

			auto combineByBfs = combination_method_factory::breadthFirstSearch(0.0);
			auto combinedSolution = combineByBfs(graph, initialSolution, initialSolution);

			assert(combinedSolution.size(), >, 0);
			for (Vertex v = 0; v < combinedSolution.size(); v++) {
				assert(combinedSolution[v], ==, initialSolution[v]);
			}
		};

		test_case("combining two equal solutions using breadth first search with 1.0 mutation probability should alter the solution") {
			MockGraph graph;
			Solution initialSolution(graph.getNumberOfVertices());

			for (Vertex v = 0; v < initialSolution.size(); v++) {
				initialSolution[v] = v*2;
			}

			auto combineByBfs = combination_method_factory::breadthFirstSearch(1.0);
			auto combinedSolution = combineByBfs(graph, initialSolution, initialSolution);

			bool foundDifferentTiming = false;
			assert(combinedSolution.size(), >, 0);
			for (Vertex v = 0; v < combinedSolution.size() && !foundDifferentTiming; v++) {
				foundDifferentTiming = combinedSolution[v] != initialSolution[v];
			}
			assert(foundDifferentTiming, ==, true);
		};

		test_case("crossovering two equal solutions without mutation probability should not alter the solution") {
			MockGraph graph;
			Solution initialSolution(graph.getNumberOfVertices());

			for (Vertex v = 0; v < initialSolution.size(); v++) {
				initialSolution[v] = v*2;
			}

			auto crossover = combination_method_factory::crossover(0.0);
			auto combinedSolution = crossover(graph, initialSolution, initialSolution);

			assert(combinedSolution.size(), >, 0);
			for (Vertex v = 0; v < combinedSolution.size(); v++) {
				assert(combinedSolution[v], ==, initialSolution[v]);
			}
		};
	}

	test_suite("when performing scatter search") {
		test_case("should throw error when total size of the population is not even") {
			MockGraph graph;
			bool exception_raised = false;
			try {
				scatterSearch(graph, 3, 4, 1, stop_function_factory::numberOfIterations(1), combination_method_factory::breadthFirstSearch(0.2));
			} catch(invalid_argument &e) {
				exception_raised = true;
			}
			assert(exception_raised, ==, true);
		};

		test_case("scatter search solution should have at least one timing different from 0") {
			MockGraph graph;
			size_t elitePopulationSize = 4;
			size_t diversePopulationSize = 8;
			size_t localSearchIterations = 10;

			auto stopFunction = stop_function_factory::numberOfIterations(3);
			auto combinationMethod = combination_method_factory::breadthFirstSearch(0.2);

			auto searchedSolution = scatterSearch(graph, elitePopulationSize, diversePopulationSize, localSearchIterations, stopFunction, combinationMethod);

			auto has_non_zero_timing = false;
			for (Vertex v = 0; v < searchedSolution.size() && !has_non_zero_timing; v++) {
				if (searchedSolution[v] != 0) {
					has_non_zero_timing = true;
				}
			}

			assert(has_non_zero_timing, ==, true);
		};

		test_case("scatter search solution should be better than solution with 0 timings") {
			MockGraph graph;
			Solution zeroTimingSolution(graph.getNumberOfVertices());

			size_t elitePopulationSize = 4;
			size_t diversePopulationSize = 8;
			size_t localSearchIterations = 10;

			auto stopFunction = stop_function_factory::numberOfIterations(3);
			auto combinationMethod = combination_method_factory::breadthFirstSearch(0.2);

			auto searchedSolution = scatterSearch(graph, elitePopulationSize, diversePopulationSize, localSearchIterations, stopFunction, combinationMethod);

			assert(graph.totalPenalty(searchedSolution), <, graph.totalPenalty(zeroTimingSolution));
		};

		test_case("scatter search solution should have all timings in the interval [0, cycle)") {
			MockGraph graph;
			size_t elitePopulationSize = 4;
			size_t diversePopulationSize = 8;
			size_t localSearchIterations = 10;

			auto stopFunction = stop_function_factory::numberOfIterations(3);
			auto combinationMethod = combination_method_factory::breadthFirstSearch(0.2);

			auto searchedSolution = scatterSearch(graph, elitePopulationSize, diversePopulationSize, localSearchIterations, stopFunction, combinationMethod);

			for (Vertex v = 0; v < searchedSolution.size(); v++) {
				auto timing = searchedSolution[v];
				assert(timing, >=, 0);
				assert(timing, <, graph.getCycle());
			}
		};
	}
};
