#include "assert.h"
#include "traffic_graph.h"

using namespace traffic;
using namespace std;

size_t numberOfTestVertices = 5;

int main (void) {
	Solution*	solution = NULL;

	test_case("solution instantiation throws no errors") {
		solution = new Solution(numberOfTestVertices);
	} end_test_case;

	test_case("set timing") {
		solution->setTiming(1, 3);
		assert_equal(solution->getTiming(1), 3);
	} end_test_case;

	test_case("unspecified timings have value 0") {
		assert_equal(solution->getTiming(0), 0);
	} end_test_case;

	test_case("destroying directly instatited solution throws no errors") {
		delete solution;
	} end_test_case;

}
