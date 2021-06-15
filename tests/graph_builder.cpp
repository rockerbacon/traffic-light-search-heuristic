#include <traffic_graph/traffic_graph.h>
#include <assertions-test/test.h>

using namespace traffic;

tests {
	test_suite("when building graph") {
		test_case("should return true when adding new edge") {
			GraphBuilder builder;
			assert(builder.addEdge({0, 1}, 3), ==, true);
			assert(builder.addEdge({6, 5}, 6), ==, true);
		};

		test_case("should return false when trying to add edge which already exists") {
			GraphBuilder builder;
			builder.addEdge({0, 1}, 2);
			assert(builder.addEdge({0, 1}, 2), ==, false);
		};

		test_case("should return false when trying to add a loop edge") {
			GraphBuilder builder;
			assert(builder.addEdge({1, 1}, 3), ==, false);
		};
	}
};

