#include "traffic_graph.h"
#include "assert.h"

using namespace traffic;
int main (void) {

	GraphBuilder* graphBuilder;

	test_case("GraphBuilder instantiation raises no errors") {
		graphBuilder = new GraphBuilder();
	} end_test_case;

	test_case("adding edge to GraphBuilder raises no errors") {
		graphBuilder->addEdge({0, 1}, 3);
		graphBuilder->addEdge({6, 5}, 6);
	} end_test_case;

	test_case("setting cycle to GraphBuilder raises no errors") {
		graphBuilder->withCycle(10);
	} end_test_case;

	test_case ("GraphBuilder destruction throws no errors") {
		delete graphBuilder;
	} end_test_case;

}
