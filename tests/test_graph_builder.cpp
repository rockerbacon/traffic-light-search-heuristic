#include "traffic_graph/traffic_graph.h"
#include "assertions/assert.h"

using namespace traffic;
int main (void) {

	GraphBuilder* graphBuilder;

	test_case("GraphBuilder instantiation raises no errors") {
		graphBuilder = new GraphBuilder();
	} end_test_case;

	test_case("adding new edge returns true") {
		assert_true(graphBuilder->addEdge({0, 1}, 3));
		assert_true(graphBuilder->addEdge({6, 5}, 6));
	} end_test_case;

	test_case("trying to add existing edge returns false") {
		assert_equal(graphBuilder->addEdge({0, 1}, 2), false);
	} end_test_case;

	test_case("trying to add edge between equal vertices returns false") {
		assert_equal(graphBuilder->addEdge({1, 1}, 3), false);
	} end_test_case;

	test_case("setting cycle to GraphBuilder raises no errors") {
		graphBuilder->withCycle(10);
	} end_test_case;

	test_case ("GraphBuilder destruction throws no errors") {
		delete graphBuilder;
	} end_test_case;

}
