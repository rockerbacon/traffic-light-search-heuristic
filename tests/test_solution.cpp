#include "assert.h"
#include "traffic_graph.h"

using namespace traffic;
using namespace std;

size_t numberOfTestVertices = 5;

int main (void) {
	Solution* solution = NULL;
	Solution* b;

	test_case("solution instantiation throws no errors") {
		solution = new Solution(numberOfTestVertices);
		b = new Solution(numberOfTestVertices);
	} end_test_case;

	test_case("number of vertices is correct") {
		assert_equal(solution->getNumberOfVertices(), numberOfTestVertices);
	} end_test_case;

	test_case("set timing") {
		solution->setTiming(1, 3);
		assert_equal(solution->getTiming(1), 3);
	} end_test_case;

	test_case("unspecified timings have value 0") {
		assert_equal(solution->getTiming(0), 0);
	} end_test_case;

	test_case("attribution between two solutions creates an identical copy") {
		*b = *solution;

		for (Vertex v = 0; v < numberOfTestVertices; v++) {
			assert_equal(b->getTiming(v), solution->getTiming(v));
		}
	} end_test_case;

	test_case("setting timing in a copy does not change the original") {
		b->setTiming(1, 5);
		assert_not_equal(b->getTiming(1), solution->getTiming(1));
	} end_test_case;

	test_case("attributing xvalue to solution raises no errors") {
		Solution xsolution(5);
		xsolution = Solution(8);
	} end_test_case;

	test_case("destroying directly instatited solution throws no errors") {
		delete solution;
		delete b;
	} end_test_case;

}
