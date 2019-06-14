#include "traffic_graph.h"
#include "assert.h"
#include <stack>
#include <iostream>

#define NUMBER_OF_VERTICES 30000
//#define MIN_VERTEX_DEGREE 2
#define MAX_VERTEX_DEGREE 20
#define MIN_EDGE_WEIGHT 2
#define MAX_EDGE_WEIGHT 13
#define CYCLE 20

using namespace std;
using namespace traffic;
int main (void) {

	GraphBuilder* graphBuilder;
	Graph* graph;

	test_case("build random adjacency list") {
		graphBuilder = new GraphBuilder();
		graphBuilder->generateRandomGraph(NUMBER_OF_VERTICES, MAX_VERTEX_DEGREE, MIN_EDGE_WEIGHT, MAX_EDGE_WEIGHT);
		graphBuilder->withCycle(CYCLE);
		graph = graphBuilder->buildAsAdjacencyList();
		assert_true(graph != NULL);
	} end_test_case;

	test_case("number of vertices")
	{
		assert_equal(graph->getNumberOfVertices(), NUMBER_OF_VERTICES);
	} end_test_case;

	test_case("max degree")
	{
		int maxDegree = -1;
		int vDegree;

		unordered_map<Vertex, Weight> vNeighbors;

		for(Vertex v = 0; v < NUMBER_OF_VERTICES; v++)
		{
			vNeighbors = graph->neighborsOf(v);
			vDegree = vNeighbors.size();

			if(vDegree > maxDegree)
			{
				maxDegree = vDegree;
			}
		}
		assert_less_than_or_equal(maxDegree, MAX_VERTEX_DEGREE);
	} end_test_case;

	test_case("graph connectivity")
	{
		int verticesCount = 0; //counting the first vertex (0) already
		int visited[NUMBER_OF_VERTICES];
		stack<Vertex> s;
		Vertex v;
		unordered_map<Vertex, Weight> vNeighbors;

		for(int i = 0; i < NUMBER_OF_VERTICES; i++)
		{
			visited[i] = 0;
		}

		s.push(0);

		while(!s.empty())
		{
			v = s.top();
			s.pop();
			visited[v] = 1;
			verticesCount++;
			vNeighbors = graph->neighborsOf(v);

			for(auto u : vNeighbors)
			{
				if(!visited[u.first])
				{
					s.push(u.first);
				}
			}			
		}

		assert_equal(verticesCount, NUMBER_OF_VERTICES);
	} end_test_case;

	test_case ("destroy GraphBuilder") {
		delete graphBuilder;
	} end_test_case;

	test_case("destroy AdjacencyMatrixGraph") {
		delete graph;
	} end_test_case;
}
