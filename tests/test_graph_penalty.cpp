#include "assert.h"
#include "traffic_graph.h"

#define EDGE1 {2, 3}
#define EDGE1_WEIGHT 6
#define CYCLE 20
#define TIMING_U 16
#define TIMING_V 8

using namespace ufrrj;

int main (void) {

	TrafficGraphBuilder* graph_builder;
	TrafficGraph* graph;

	Vertice u, v;
	u = 2;
	v = 3;

	graph_builder = new TrafficGraphBuilder();
	graph_builder->addEdge(EDGE1, EDGE1_WEIGHT);
	graph_builder->withCycle(CYCLE);

	graph = graph_builder->buildAsAdjacencyMatrix();

	test_case("set vertice timing") {
		graph->setTiming(u, TIMING_U);
		graph->setTiming(v, TIMING_V);

		assert_equal(graph->getTiming(u), TIMING_U);
		assert_equal(graph->getTiming(v), TIMING_V);
	} end_test_case;

	test_case("penalty between two vertices with edge between them") {
		int penalty_uv = graph->penalty(u, v);
		int penalty_vu = graph->penalty(v, u);
		assert_equal(penalty_uv, 6);
		assert_equal(penalty_vu, 2);
	} end_test_case;

	test_case ("penalty between two vertices with no edge between them") {
		int penalty = graph->penalty(0, 1);
		assert_equal(penalty, 0);
	} end_test_case;

	test_case("TrafficGraphBuilder destruction throws no errors") {
		delete graph_builder;
	} end_test_case;

	test_case("AdjacencyMatrixGraph destruction throws no errors") {
		delete graph;
	} end_test_case;
}
