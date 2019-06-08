#include "traffic_graph.h"
#include "assert.h"

using namespace ufrrj;
int main (void) {

	TrafficGraphBuilder* graphBuilder;

	test_case("TrafficGraphBuilder instantiation raises no errors") {
		graphBuilder = new TrafficGraphBuilder();
	} end_test_case;

	test_case("adding edge to TrafficGraphBuilder raises no errors") {
		graphBuilder->addEdge({0, 1}, 3);
		graphBuilder->addEdge({6, 5}, 6);
	} end_test_case;

	test_case("setting cycle to TrafficGraphBuilder raises no errors") {
		graphBuilder->withCycle(10);
	} end_test_case;

	test_case ("TrafficGraphBuilder destruction throws no errors") {
		delete graphBuilder;
	} end_test_case;

}
