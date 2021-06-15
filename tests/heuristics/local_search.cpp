#include <assertions-test/test.h>
#include <traffic_graph/traffic_graph.h>
#include <heuristic/heuristic.h>
#include "mock_graph.h"

using namespace traffic;
using namespace std;
using namespace heuristic;

tests {
	test_suite("when performing a local search") {
		test_case("searched solution should be different from initial solution") {
			MockGraph graph;
			auto initialSolution = Solution(graph.getNumberOfVertices());
			auto searchedSolution = localSearchHeuristic(graph, initialSolution, stop_function_factory::numberOfIterations(25));
			bool searched_solution_is_different = false;
			for (Vertex v = 0; v < graph.getNumberOfVertices(); v++) {
				if (searchedSolution[v] != initialSolution[v]) {
					searched_solution_is_different = true;
				}
			}
			assert(searched_solution_is_different, ==, true);
		};

		test_case("searched solution should be better than initial solution") {
			MockGraph graph;
			auto initialSolution = Solution(graph.getNumberOfVertices());
			auto searchedSolution = localSearchHeuristic(graph, initialSolution, stop_function_factory::numberOfIterations(25));
			assert(graph.totalPenalty(searchedSolution), <, graph.totalPenalty(initialSolution));
		};

		test_case("searched solution should have all timings in the interval [0, cycle)") {
			MockGraph graph;
			auto initialSolution = Solution(graph.getNumberOfVertices());
			auto searchedSolution = localSearchHeuristic(graph, initialSolution, stop_function_factory::numberOfIterations(25));
			for (Vertex v = 0; v < graph.getNumberOfVertices(); v++) {
				auto timing = searchedSolution[v];
				assert(timing, >=, 0);
				assert(timing, <, graph.getCycle());
			}
		};
	}
};
