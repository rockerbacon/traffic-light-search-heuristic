#include <assertions-test/test.h>
#include <traffic_graph/traffic_graph.h>

using namespace traffic;
using namespace std;

#define NUMBER_OF_VERTICES 5

begin_tests {
	test_suite("when instantiating solution") {
		test_case("solution should have the correct number of vertices") {
			Solution solution(NUMBER_OF_VERTICES);
			assert(solution.getNumberOfVertices(), ==, NUMBER_OF_VERTICES);
		};

		test_case("all timings are initially set to 0") {
			Solution solution(NUMBER_OF_VERTICES);
			for (Vertex v = 0; v < solution.getNumberOfVertices(); v++) {
				assert(solution.getTiming(0), ==, 0);
			}
		};

		test_case("attribution between two solutions should create an identical copy") {
			Solution a(NUMBER_OF_VERTICES);
			Solution b;
			a.setTiming(0, 2);
			a.setTiming(2, 10);

			b = a;

			for (Vertex v = 0; v < a.getNumberOfVertices(); v++) {
				assert(b.getTiming(v), ==, a.getTiming(v));
			}
		};

		test_case("should be able to attribute from an xvalue") {
			Solution solution(5);
			solution = Solution(8);
			assert(solution.getNumberOfVertices(), ==, 8);
		};
	}

	test_suite("when managing solution timings") {
		test_case("should set timings correctly") {
			Solution solution(NUMBER_OF_VERTICES);
			solution.setTiming(1, 3);
			assert(solution.getTiming(1), ==, 3);
		};

		test_case("setting timing in a copy should not change the original") {
			Solution original(NUMBER_OF_VERTICES);
			Solution copy(original);
			copy.setTiming(1, 5);
			assert(copy.getTiming(1), !=, original.getTiming(1));
		};
	}
} end_tests;
