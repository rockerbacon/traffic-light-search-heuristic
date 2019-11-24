#include <assertions-test/test.h>
#include <traffic_graph/traffic_graph.h>

#define NUMBER_OF_VERTICES 8

#define EDGE_7_5_FIXTURE Graph::Edge{7, 5}
#define EDGE_7_5_WEIGHT_FIXTURE 6

#define EDGE_5_2_FIXTURE Graph::Edge{5, 2}
#define EDGE_5_2_WEIGHT_FIXTURE 10

#define EDGE_5_4_FIXTURE Graph::Edge{5, 4}
#define EDGE_5_4_WEIGHT_FIXTURE 4

#define CYCLE 20
#define TIMING_7 16
#define TIMING_5 8
#define TIMING_2 4
#define TIMING_4 7

using namespace traffic;
using namespace std;

class MockGraph : public Graph {
	private:
		unordered_map<Vertex, Weight> neighborsOf2;
		unordered_map<Vertex, Weight> neighborsOf4;
		unordered_map<Vertex, Weight> neighborsOf5;
		unordered_map<Vertex, Weight> neighborsOf7;
		unordered_map<Vertex, Weight> emptyMap;
	public:
		MockGraph() : Graph(NUMBER_OF_VERTICES, CYCLE) {
			this->neighborsOf2 = { {5, 10} };
			this->neighborsOf4 = { {5, 4} };
			this->neighborsOf5 = { {2, 10}, {4, 4}, {7, 6} };
			this->neighborsOf7 = { {5, 6} };
		}

		virtual Weight weight(const Graph::Edge& edge) const {
			if (edge == EDGE_7_5_FIXTURE) {
				return EDGE_7_5_WEIGHT_FIXTURE;
			} else if (edge == EDGE_5_2_FIXTURE) {
				return EDGE_5_2_WEIGHT_FIXTURE;
			} else if (edge == EDGE_5_4_FIXTURE) {
				return EDGE_5_4_WEIGHT_FIXTURE;
			} else {
				return -1;
			}
		}

		virtual const unordered_map<Vertex, Weight>& neighborsOf (Vertex vertex) const {
			switch(vertex) {
				case 2:
					return this->neighborsOf2;
					break;
				case 4:
					return this->neighborsOf4;
					break;
				case 5:
					return this->neighborsOf5;
					break;
				case 7:
					return this->neighborsOf7;
					break;
				default:
					return this->emptyMap;
			}
		}
};

Solution solutionFixture() {
	Solution solution(NUMBER_OF_VERTICES);
	solution[7] = TIMING_7;
	solution[5] = TIMING_5;
	solution[2] = TIMING_2;
	solution[4] = TIMING_4;
	return solution;
}

begin_tests {
	test_suite("after instantiating object of class deriving from Graph") {
		test_case("graph should report the correct number of vertices") {
			MockGraph graph;
			assert(graph.getNumberOfVertices(), ==, NUMBER_OF_VERTICES);
		};

		test_case("graph should have the correct cycle") {
			MockGraph graph;
			assert(graph.getCycle(), ==, CYCLE);
		};
	}

	test_suite("when calculating penalties") {
		test_case("penalty between two vertices with edge between them should be calculated correctly") {
			MockGraph graph;
			auto u = EDGE_7_5_FIXTURE.vertex1;
			auto v = EDGE_7_5_FIXTURE.vertex2;
			assert(graph.penalty(u, v, solutionFixture()), ==, 6);
			assert(graph.penalty(v, u, solutionFixture()), ==, 2);
		};

		test_case ("penalty between two vertices with no edge between them should be 0") {
			MockGraph graph;
			for (size_t i = 0; i < NUMBER_OF_VERTICES; i++) {
				for (size_t j = 0; j < NUMBER_OF_VERTICES; j++) {
					Graph::Edge edge{i, j};
					if (edge == EDGE_7_5_FIXTURE || edge == EDGE_5_2_FIXTURE || edge == EDGE_5_4_FIXTURE) {
						continue;
					}
					assert(graph.penalty(i, j, solutionFixture()), ==, 0);
				}
			}
		};

		test_case ("penalty for a vertex should be calculated correctly") {
			MockGraph graph;
			TimeUnit expectedPenalty = 2+6+5;
			assert(graph.vertexPenaltyOnewayOnly(5, solutionFixture()), ==, expectedPenalty);
		};

		test_case ("penalty for a vertex and its neighbors should be calculated correctly") {
			MockGraph graph;
			TimeUnit expectedPenalty = 2+6+5 + 6+6+3;
			assert(graph.vertexPenalty(5, solutionFixture()), ==, expectedPenalty);
		};

		test_case ("penalty for the the entire graph should be calculated correctly") {
			MockGraph graph;
			TimeUnit expectedPenalty = 28;
			assert(graph.totalPenalty(solutionFixture()), ==, expectedPenalty);
		};

		test_case("graph lower bound should be calculated correctly") {
			MockGraph graph;
			assert(graph.lowerBound(), ==, 16);
		};
	}

	/*
	test_case("set vertices timings in solution") {
		solution.setTiming(7, TIMING_7);
		solution.setTiming(5, TIMING_5);
		solution.setTiming(2, TIMING_2);
		solution.setTiming(4, TIMING_4);
		assert_equal(solution.getTiming(edge1.vertex1), TIMING_7);
		assert_equal(solution.getTiming(edge1.vertex2), TIMING_5);
	} end_test_case;

	test_case("edge (u,v) equals edge (v,u)") {
		Graph::Edge a, b;
		a = {1, 2};
		b = {2, 1};
		assert_true(a == b);
	} end_test_case;
	*/
} end_tests;
