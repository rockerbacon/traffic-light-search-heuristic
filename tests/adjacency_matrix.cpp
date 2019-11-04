#include <traffic_graph/traffic_graph.h>
#include <assertions-test/test.h>

#define NUMBER_OF_VERTICES 8

#define EDGE_7_5_FIXTURE Graph::Edge{7, 5}
#define EDGE_5_7_FIXTURE Graph::Edge{5, 7}
#define EDGE_7_5_WEIGHT_FIXTURE 6

#define EDGE_2_3_FIXTURE Graph::Edge{2, 3}
#define EDGE_3_2_FIXTURE Graph::Edge{3, 2}
#define EDGE_2_3_WEIGHT_FIXTURE 3

#define EDGE_2_4_FIXTURE Graph::Edge{2, 4}
#define EDGE_4_2_FIXTURE Graph::Edge{4, 2}
#define EDGE_2_4_WEIGHT_FIXTURE 13

#define EDGE_2_0_FIXTURE Graph::Edge{2, 0}
#define EDGE_0_2_FIXTURE Graph::Edge{0, 2}
#define EDGE_2_0_WEIGHT_FIXTURE 16

#define CYCLE 20
#define TIMING_U 16
#define TIMING_V 8

using namespace traffic;

AdjacencyMatrixGraph* graphFixture(void) {
	GraphBuilder graphBuilder;
	graphBuilder.addEdge(EDGE_7_5_FIXTURE, EDGE_7_5_WEIGHT_FIXTURE);
	graphBuilder.addEdge(EDGE_2_3_FIXTURE, EDGE_2_3_WEIGHT_FIXTURE);
	graphBuilder.addEdge(EDGE_2_4_FIXTURE, EDGE_2_4_WEIGHT_FIXTURE);
	graphBuilder.addEdge(EDGE_2_0_FIXTURE, EDGE_2_0_WEIGHT_FIXTURE);
	graphBuilder.withCycle(CYCLE);
	return graphBuilder.buildAsAdjacencyMatrix();
}

begin_tests {

	test_suite("instantiation and destruction") {
		test_case("graph can be correctly built from a graph builder and then destroyed") {
			auto graph = graphFixture();
			assert(graph, !=, NULL);
			delete graph;
		};
	}

	test_suite("when adding edges") {
		test_case("added edges should have correct weight") {
			auto graph = graphFixture();
			assert(graph->weight(EDGE_7_5_FIXTURE), ==, EDGE_7_5_WEIGHT_FIXTURE);
			assert(graph->weight(EDGE_2_3_FIXTURE), ==, EDGE_2_3_WEIGHT_FIXTURE);
			assert(graph->weight(EDGE_2_4_FIXTURE), ==, EDGE_2_4_WEIGHT_FIXTURE);
			assert(graph->weight(EDGE_2_0_FIXTURE), ==, EDGE_2_0_WEIGHT_FIXTURE);
			delete graph;
		};

		test_case("weight for edge (u,v) should equal weight for edge (v,u)") {
			auto graph = graphFixture();
			assert(graph->weight(EDGE_5_7_FIXTURE), ==, EDGE_7_5_WEIGHT_FIXTURE);
			assert(graph->weight(EDGE_3_2_FIXTURE), ==, EDGE_2_3_WEIGHT_FIXTURE);
			assert(graph->weight(EDGE_4_2_FIXTURE), ==, EDGE_2_4_WEIGHT_FIXTURE);
			assert(graph->weight(EDGE_0_2_FIXTURE), ==, EDGE_2_0_WEIGHT_FIXTURE);
			delete graph;
		};

		test_case("non existing edges should have weight -1") {
			auto graph = graphFixture();
			for (Vertex vertex1 = 0; vertex1 < NUMBER_OF_VERTICES; vertex1++) {
				for (Vertex vertex2 = 0; vertex2 < NUMBER_OF_VERTICES; vertex2++) {
					Graph::Edge unexistentEdge = {vertex1, vertex2};
					if	(
							   unexistentEdge == EDGE_7_5_FIXTURE || unexistentEdge == EDGE_5_7_FIXTURE
							|| unexistentEdge == EDGE_2_3_FIXTURE || unexistentEdge == EDGE_3_2_FIXTURE
							|| unexistentEdge == EDGE_2_4_FIXTURE || unexistentEdge == EDGE_4_2_FIXTURE
							|| unexistentEdge == EDGE_2_0_FIXTURE || unexistentEdge == EDGE_0_2_FIXTURE
						) {
						continue;
					} else {
						assert(graph->weight(unexistentEdge), ==, -1);
					}
				}
			}
			delete graph;
		};
	}

	test_suite("when using neighborhoods") {

		test_case("neighborhood should correctly have all neighbor vertices") {
			auto graph = graphFixture();
			bool vertex_0_present = false,
				 vertex_3_present = false,
				 vertex_4_present = false;

			for (auto& neighbor : graph->neighborsOf(2)) {
				vertex_0_present = vertex_0_present || neighbor.first == 0;
				vertex_3_present = vertex_3_present || neighbor.first == 3;
				vertex_4_present = vertex_4_present || neighbor.first == 4;
			}

			assert(vertex_0_present, ==, true);
			assert(vertex_3_present, ==, true);
			assert(vertex_4_present, ==, true);

			delete graph;
		};

		test_case("neighborhood should have correct edge weights") {
			auto graph = graphFixture();
			TimeUnit edge_2_0_weight = -1,
					 edge_2_3_weight = -1,
					 edge_2_4_weight = -1;

			for (auto& neighbor : graph->neighborsOf(2)) {
				switch (neighbor.first) {
					case 0:
						edge_2_0_weight = neighbor.second;
					break;
					case 3:
						edge_2_3_weight = neighbor.second;
					break;
					case 4:
						edge_2_4_weight = neighbor.second;
					break;
				}
			}

			assert(edge_2_0_weight, ==, EDGE_2_0_WEIGHT_FIXTURE);
			assert(edge_2_3_weight, ==, EDGE_2_3_WEIGHT_FIXTURE);
			assert(edge_2_4_weight, ==, EDGE_2_4_WEIGHT_FIXTURE);
			delete graph;
		};

		test_case("if u is in the neighborhood of v then v should be in the neighborhood of u") {
			auto graph = graphFixture();
			Vertex u = 2;
			Vertex v = 3;
			auto neighbors_of_v = graph->neighborsOf(v);
			auto neighbors_of_u = graph->neighborsOf(u);

			assert(neighbors_of_v.find(u), !=, neighbors_of_v.end());
			assert(neighbors_of_u.find(v), !=, neighbors_of_u.end());

			delete graph;
		};

	}
} end_tests;
