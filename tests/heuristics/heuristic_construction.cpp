#include <assertions-test/test.h>
#include <traffic_graph/traffic_graph.h>
#include <heuristic/heuristic.h>
#include "mock_graph.h"

using namespace traffic;
using namespace std;
using namespace heuristic;

tests {
	test_suite("when constructing heuristic initial solution") {
		test_case("solution should have at least one timing that's not 0") {
			MockGraph graph;
			auto solution = constructHeuristicSolution(graph);
			bool has_non_zero_timing = false;
			for (Vertex i = 0; i < graph.getNumberOfVertices() && !has_non_zero_timing; i++) {
				if (solution[i] != 0) {
					has_non_zero_timing = true;
				}
			}
			assert(has_non_zero_timing, ==, true);
		};

		test_case("solution should have all timings in the interval [0, cycle)") {
			MockGraph graph;
			auto solution = constructHeuristicSolution(graph);
			for (Vertex i = 0; i < graph.getNumberOfVertices(); i++) {
				auto timing = solution[i];
				assert((timing >= 0 && timing < testCycle), ==, true);
			}
		};
	}
};
