#include "assert.h"
#include "traffic_graph.h"

using namespace ufrrj;
int main (void) {
	Solution* solution = NULL;

	test_case("instantiation throws no errors") {
		solution = new Solution(5);
	} end_test_case;

	test_case("set timing") {
		solution->setTiming(1, 3);
		assert_equal(solution->getTiming(1), 3);
	} end_test_case;

	test_case("unspecified timings have value 0") {
		assert_equal(solution->getTiming(0), 0);
	} end_test_case;

	delete solution;
}
