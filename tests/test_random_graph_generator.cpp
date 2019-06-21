#include "traffic_graph.h"
#include "assert.h"
#include <queue>
#include <iostream>

#define NUMBER_OF_VERTICES 30
#define MIN_VERTEX_DEGREE 5
#define MAX_VERTEX_DEGREE 10
#define MIN_EDGE_WEIGHT 2
#define MAX_EDGE_WEIGHT 13
#define CYCLE 20

using namespace std;
using namespace traffic;
int main (void) {

	GraphBuilder* graphBuilder;
	Graph* graph;

	test_case("build random adjacency list") {
		graphBuilder = new GraphBuilder(NUMBER_OF_VERTICES, MIN_VERTEX_DEGREE, MAX_VERTEX_DEGREE, MIN_EDGE_WEIGHT, MAX_EDGE_WEIGHT);
		graphBuilder->withCycle(CYCLE);
		graph = graphBuilder->buildAsAdjacencyList();
		assert_true(graph != NULL);
	} end_test_case;

	test_case("number of vertices")
	{
		assert_equal(graph->getNumberOfVertices(), NUMBER_OF_VERTICES);
	} end_test_case;

	test_case("no vertex has degree higher than the maximum degree")
	{
		int vDegree;

		unordered_map<Vertex, Weight> vNeighbors;

		for(Vertex v = 0; v < NUMBER_OF_VERTICES; v++)
		{
			vNeighbors = graph->neighborsOf(v);
			vDegree = vNeighbors.size();

			assert_less_than_or_equal(vDegree, MAX_VERTEX_DEGREE);
		}
	} end_test_case;

	test_case("graph is connected")
	{
		int verticesCount;
		bool counted[NUMBER_OF_VERTICES];
		queue<Vertex> s;
		Vertex v;
		unordered_map<Vertex, Weight> vNeighbors;

		for(int i = 1; i < NUMBER_OF_VERTICES; i++)
		{
			counted[i] = false;
		}

		s.push(0);
		counted[0] = true;
		verticesCount = 1;

		while(!s.empty())
		{
			v = s.front();
			s.pop();
			vNeighbors = graph->neighborsOf(v);

			for(auto u : vNeighbors)
			{
				if(!counted[u.first])
				{
					s.push(u.first);
					counted[u.first] = true;
					verticesCount++;
				}
			}
		}

		assert_equal(verticesCount, NUMBER_OF_VERTICES);
	} end_test_case;

	test_case("no vertex has degree less than the minimum degree") {
		for (Vertex v = 0; v < graph->getNumberOfVertices(); v++) {
			assert_greater_than_or_equal(graph->neighborsOf(v).size(), MIN_VERTEX_DEGREE)
		}
	} end_test_case;

	test_case ("destroy GraphBuilder") {
		delete graphBuilder;
	} end_test_case;

	test_case("destroy AdjacencyMatrixGraph") {
		delete graph;
	} end_test_case;
}
