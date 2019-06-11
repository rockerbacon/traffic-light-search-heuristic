#include "assert.h"
#include "traffic_graph.h"

using namespace traffic;

TimeUnit cycle = 20;
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

int main (void) {
	Solution*	solution = NULL;
	Solution*	constructedSolution = NULL;
	GraphBuilder* builder;
	AdjacencyListGraph* adjacencyList = NULL;
	AdjacencyMatrixGraph* adjacencyMatrix = NULL;

	test_case("solution instantiation throws no errors") {
		solution = new Solution(5);
	} end_test_case;

	test_case("building graphs throws no errors") {
		builder = new GraphBuilder();
		builder->addEdge(edge1, weight1);
		builder->addEdge(edge2, weight2);
		builder->addEdge(edge3, weight3);
		builder->addEdge(edge4, weight4);
		builder->addEdge(edge5, weight5);
		builder->addEdge(edge6, weight6);
		builder->withCycle(cycle);
		adjacencyList = builder->buildAsAdjacencyList();
		adjacencyMatrix = builder->buildAsAdjacencyMatrix();
		assert_true(adjacencyList != NULL);
		assert_true(adjacencyMatrix != NULL);
	} end_test_case;

	test_case("set timing") {
		solution->setTiming(1, 3);
		assert_equal(solution->getTiming(1), 3);
	} end_test_case;

	test_case("unspecified timings have value 0") {
		assert_equal(solution->getTiming(0), 0);
	} end_test_case;

	test_case("creating initial solution with adjacency list throws no errors") {
		constructedSolution = constructSolution(*adjacencyList);
		assert_true(constructedSolution != NULL);
	} end_test_case;

	test_case("solution created with adjacency list has at least one timming that's not 0") {
		bool found = false;
		for (size_t i = 0; i < adjacencyList->getNumberOfVertices() && !found; i++) {
			if (constructedSolution->getTiming(i) != 0) {
				found = true;
			}
		}
		assert_true(found);
	} end_test_case;

	test_case("solution created with adjacency list has all timmings in the interval [0, T)") {
		TimeUnit timming;
		for (size_t i = 0; i < adjacencyList->getNumberOfVertices(); i++) {
			timming = constructedSolution->getTiming(i);
			assert_true(timming >= 0 && timming < cycle);
		}
	} end_test_case;

	test_case("destroying solution created with adjacency list throws no errors") {
		delete constructedSolution;
	} end_test_case;

	test_case("creating initial solution with adjacency matrix throws no errors") {
		constructedSolution = constructSolution(*adjacencyMatrix);
		assert_true(constructedSolution != NULL);
	} end_test_case;

	test_case("solution created with adjacency matrix has at least one timming that's not 0") {
		bool found = false;
		for (size_t i = 0; i < adjacencyMatrix->getNumberOfVertices() && !found; i++) {
			if (constructedSolution->getTiming(i) != 0) {
				found = true;
			}
		}
		assert_true(found);
	} end_test_case;

	test_case("solution created with adjacency matrix has all timmings in the interval [0, T)") {
		TimeUnit timming;
		for (size_t i = 0; i < adjacencyMatrix->getNumberOfVertices(); i++) {
			timming = constructedSolution->getTiming(i);
			assert_true(timming >= 0 && timming < cycle);
		}
	} end_test_case;

	test_case("destroying solution created with adjacency matrix throws no errors") {
		delete constructedSolution;
	} end_test_case;

	test_case("destroying directly instatited solution throws no errors") {
		delete solution;
	} end_test_case;

	test_case("destroy GraphBuilder") {
		delete builder;
	} end_test_case;

	test_case ("destroy AdjacencyListGraph") {
		delete adjacencyList;
	} end_test_case;

	test_case("destroy AdjacencyMatrixGraph") {
		delete adjacencyMatrix;
	} end_test_case;
}
