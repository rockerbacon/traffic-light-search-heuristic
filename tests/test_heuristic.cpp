#include "assertions/assert.h"
#include "traffic_graph/traffic_graph.h"
#include "heuristic/heuristic.h"

using namespace traffic;
using namespace std;
using namespace heuristic;

TimeUnit testCycle = 20;
size_t numberOfTestVertices = 5;
Graph::Edge	edge1 = {0, 1},
			edge2 = {1, 2},
			edge3 = {2, 3},
			edge4 = {3, 0},
			edge5 = {0, 4},
			edge6 = {4, 2};
Weight	weight1 = 7,
		weight2 = 12,
		weight3 = 19,
		weight4 = 3,
		weight5 = 10,
		weight6 = 6;

class MockGraph : public Graph {
	private:
		mutable unordered_map<Vertex, unordered_map<Vertex, Weight>> adjacencyList;
	public:
		MockGraph (void) : Graph(numberOfTestVertices, testCycle) {
			this->adjacencyList[0][1] = weight1;
			this->adjacencyList[1][0] = weight1;

			this->adjacencyList[1][2] = weight2;
			this->adjacencyList[2][1] = weight2;

			this->adjacencyList[2][3] = weight3;
			this->adjacencyList[3][2] = weight3;

			this->adjacencyList[3][0] = weight4;
			this->adjacencyList[0][3] = weight4;

			this->adjacencyList[0][4] = weight5;
			this->adjacencyList[4][0] = weight5;

			this->adjacencyList[4][2] = weight6;
			this->adjacencyList[2][4] = weight6;
		}
		virtual Weight weight(const Edge& edge) const {
			if (edge == edge1) {
				return weight1;
			} else if (edge == edge2) {
				return weight2;
			} else if (edge == edge3) {
				return weight3;
			} else if (edge == edge4) {
				return weight4;
			} else if (edge == edge5) {
				return weight5;
			} else if (edge == edge6) {
				return weight6;
			} else {
				return -1;
			}
		}
		virtual const std::unordered_map<Vertex, Weight>& neighborsOf(Vertex vertex) const {
			return this->adjacencyList[vertex];
		}
};

int main (void) {
	MockGraph mockGraph;
	Solution solution, initialSolution;

	test_case("creating random initial solution throws no errors") {
		solution = constructRandomSolution(mockGraph);
	} end_test_case;

	test_case("random solution has at least one timing that's not 0") {
		bool found = false;
		for (Vertex i = 0; i < mockGraph.getNumberOfVertices() && !found; i++) {
			if (solution.getTiming(i) != 0) {
				found = true;
			}
		}
		assert_true(found);
	} end_test_case;

	test_case("random solution has all timings in the interval [0, cycle)") {
		TimeUnit timing;
		for (Vertex i = 0; i < mockGraph.getNumberOfVertices(); i++) {
			timing = solution.getTiming(i);
			assert_true(timing >= 0 && timing < testCycle);
		}
	} end_test_case;

	test_case("creating heuristic initial solution throws no errors") {
		solution = constructHeuristicSolution(mockGraph);
	} end_test_case;

	test_case("constructed heuristic solution has at least one timing that's not 0") {
		bool found = false;
		for (Vertex i = 0; i < mockGraph.getNumberOfVertices() && !found; i++) {
			if (solution.getTiming(i) != 0) {
				found = true;
			}
		}
		assert_true(found);
	} end_test_case;

	test_case("constructed heuristic solution has all timings in the interval [0, cycle)") {
		TimeUnit timing;
		for (Vertex i = 0; i < mockGraph.getNumberOfVertices(); i++) {
			timing = solution.getTiming(i);
			assert_greater_than_or_equal(timing, 0);
			assert_less_than(timing, testCycle);
		}
	} end_test_case;

	test_case("distance between two equal solutions is 0") {
		Solution a(numberOfTestVertices), b(numberOfTestVertices);
		for (Vertex i = 0; i < numberOfTestVertices; i++) {
			a.setTiming(i, i);
			b.setTiming(i, i);
		}
		assert_equal(distance(mockGraph, a, b), 0);
	} end_test_case;

	test_case("distance between two solutions with only one timing is correctly calculated") {
		Solution a(numberOfTestVertices), b(numberOfTestVertices);
		a.setTiming(1, testCycle-2);
		b.setTiming(1, 1);
		assert_equal(distance(mockGraph, a, b), 3);
	} end_test_case;

	test_case("distance between two solutions with multiple timings is correctly calculated") {
		Solution a(numberOfTestVertices), b(numberOfTestVertices);
		//3
		a.setTiming(0, 18);
		b.setTiming(0, 1);
		//4
		a.setTiming(1, 4);
		b.setTiming(1, 8);
		//10
		a.setTiming(2, 10);
		b.setTiming(2, 0);

		assert_equal(distance(mockGraph, a, b), 3+4+10);
	} end_test_case;

	test_case ("reseting solution raises no errors") {
		solution = Solution(mockGraph.getNumberOfVertices());
	} end_test_case;

	test_case("local search raises no errors") {
		initialSolution = Solution(numberOfTestVertices);
		for (Vertex v = 0; v < numberOfTestVertices; v++) {
			if (v&1) {
				initialSolution.setTiming(v, 0);
			} else {
				initialSolution.setTiming(v, testCycle-1);
			}
		}
		solution = localSearchHeuristic(mockGraph, initialSolution, stop_function_factory::numberOfIterations(25));
	} end_test_case;

	test_case("searched solution is different from initial solution") {
		bool found = false;
		for (Vertex v = 0; v < mockGraph.getNumberOfVertices() && !found; v++) {
			if (solution.getTiming(v) != initialSolution.getTiming(v)) {
				found = true;
			}
		}
		assert_true(found);
	} end_test_case;

	test_case("searched solution is better than initial solution") {
		assert_less_than(mockGraph.totalPenalty(solution), mockGraph.totalPenalty(initialSolution));
	} end_test_case;

	test_case("searched solution has all timings in the interval [0, cycle)") {
		TimeUnit timing;
		for (Vertex v = 0; v < mockGraph.getNumberOfVertices(); v++) {
			timing = solution.getTiming(v);
			assert_greater_than_or_equal(timing, 0)
			assert_less_than(timing, testCycle);
		}
	} end_test_case;

	test_case("scatter search solution throws error when total size of the population is not even") {
		try {
			solution = scatterSearch(mockGraph, 3, 4, 1, stop_function_factory::numberOfIterations(1), combination_method_factory::breadthFirstSearch());
		} catch(invalid_argument &e) {
		}
	} end_test_case;

	test_case("scatter search raises no errors") {
		size_t elitePopulationSize = 4;
		size_t diversePopulationSize = 8;
		size_t localSearchIterations = 10;
		solution = scatterSearch(mockGraph, elitePopulationSize, diversePopulationSize, localSearchIterations, stop_function_factory::numberOfIterations(3), combination_method_factory::breadthFirstSearch());
	} end_test_case;


	test_case("scatter search solution is different from random solution") {
		bool found = false;
		initialSolution = constructRandomSolution(mockGraph);
		for (Vertex v = 0; v < mockGraph.getNumberOfVertices() && !found; v++) {
			if (solution.getTiming(v) != initialSolution.getTiming(v)) {
				found = true;
			}
		}
		assert_true(found);
	} end_test_case;

	test_case("scatter search solution is better than solution with 0 timings") {
		initialSolution = Solution(mockGraph.getNumberOfVertices());
		assert_less_than(mockGraph.totalPenalty(solution), mockGraph.totalPenalty(initialSolution));
	} end_test_case;

	test_case("scatter search solution has all timings in the interval [0, cycle)") {
		TimeUnit timing;
		for (Vertex v = 0; v < mockGraph.getNumberOfVertices(); v++) {
			timing = solution.getTiming(v);
			assert_greater_than_or_equal(timing, 0);
			assert_less_than(timing, testCycle);
		}
	} end_test_case;

	test_case("combining two equal solutions using breadth first search returns the unaltered solution") {
		CombinationMethod combineByBfs = combination_method_factory::breadthFirstSearch();
		Solution s = combineByBfs(mockGraph, &solution, &solution);

		for (Vertex v = 0; v < mockGraph.getNumberOfVertices(); v++)
		{
			assert_equal(solution.getTiming(v), s.getTiming(v));
		}

	} end_test_case;

	test_case("crossovering two equal solutions returns unaltered solution when mutation probability is 0") {
		CombinationMethod crossover = combination_method_factory::crossover(0.0);
		Solution s = crossover(mockGraph, &solution, &solution);

		for (Vertex v = 0; v < mockGraph.getNumberOfVertices(); v++)
		{
			assert_equal(solution.getTiming(v), s.getTiming(v));
		}

	} end_test_case;

}
