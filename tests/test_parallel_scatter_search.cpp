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

	test_case("solution throws error when total size of the population is not even") {
		try {
			solution = parallel::scatterSearch(mockGraph, 3, 4, 1, stop_function_factory::numberOfIterations(1), combination_method_factory::breadthFirstSearch(), 2);
		} catch(invalid_argument &e) {
		}
	} end_test_case;

	test_case("scatter search raises no errors") {
		size_t elitePopulationSize = 4;
		size_t diversePopulationSize = 8;
		size_t localSearchIterations = 10;
		StopFunction stopFunction = stop_function_factory::numberOfIterations(3);
		CombinationMethod combinationMethod = combination_method_factory::breadthFirstSearch();
		unsigned numberOfThreads = 4;

		solution = parallel::scatterSearch(mockGraph, elitePopulationSize, diversePopulationSize, localSearchIterations, stopFunction, combinationMethod, numberOfThreads);
	} end_test_case;

	test_case("solution is different from random solution") {
		bool found = false;
		initialSolution = constructRandomSolution(mockGraph);
		for (Vertex v = 0; v < mockGraph.getNumberOfVertices() && !found; v++) {
			if (solution.getTiming(v) != initialSolution.getTiming(v)) {
				found = true;
			}
		}
		assert_true(found);
	} end_test_case;

	test_case("solution is better than solution with 0 timings") {
		initialSolution = Solution(mockGraph.getNumberOfVertices());
		assert_less_than(mockGraph.totalPenalty(solution), mockGraph.totalPenalty(initialSolution));
	} end_test_case;

	test_case("solution has all timings in the interval [0, cycle)") {
		TimeUnit timing;
		for (Vertex v = 0; v < mockGraph.getNumberOfVertices(); v++) {
			timing = solution.getTiming(v);
			assert_greater_than_or_equal(timing, 0);
			assert_less_than(timing, testCycle);
		}
	} end_test_case;

}
