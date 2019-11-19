#include <traffic_graph/traffic_graph.h>
#include <assertions-test/test.h>
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

Graph* randomGraphFixture() {
	auto graphBuilder = GraphBuilder(NUMBER_OF_VERTICES, MIN_VERTEX_DEGREE, MAX_VERTEX_DEGREE, MIN_EDGE_WEIGHT, MAX_EDGE_WEIGHT);
	graphBuilder.withCycle(CYCLE);
	return graphBuilder.buildAsAdjacencyList();
}

begin_tests {
	test_suite("when building random graph") {
		test_case("graph should have correct number of vertices") {
			auto graph = randomGraphFixture();
			assert(graph->getNumberOfVertices(), ==, NUMBER_OF_VERTICES);
			delete graph;
		};

		test_case("vertices should have degree in interval [minVertexDegree, maxVertexDegree]") {
			auto graph = randomGraphFixture();
			for(Vertex v = 0; v < graph->getNumberOfVertices(); v++)
			{
				auto neighborhood = graph->neighborsOf(v);
				auto vertexDegree = neighborhood.size();
				assert(vertexDegree, <=, MAX_VERTEX_DEGREE);
				assert(vertexDegree, >=, MIN_VERTEX_DEGREE);
			}
			delete graph;
		};

		test_case("edges should have weight in the interval [minEdgeWeight, maxEdgeWeight]") {
			auto graph = randomGraphFixture();
			for (Vertex u = 0; u < NUMBER_OF_VERTICES; u++) {
				for (auto v : graph->neighborsOf(u)) {
					auto edgeWeight = v.second;
					assert(edgeWeight, <=, MAX_EDGE_WEIGHT);
					assert(edgeWeight, >=, MIN_EDGE_WEIGHT);
				}
			}
			delete graph;
		};

		test_case("graph should be connected") {
			auto graph = randomGraphFixture();
			int connectedVertices;
			bool visited[NUMBER_OF_VERTICES];
			queue<Vertex> vertexQueue;
			Vertex vertex;
			unordered_map<Vertex, Weight> vNeighbors;

			for(int i = 1; i < NUMBER_OF_VERTICES; i++)	{
				visited[i] = false;
			}

			vertexQueue.push(0);
			visited[0] = true;
			connectedVertices = 1;

			while(!vertexQueue.empty())	{
				vertex = vertexQueue.front();
				vertexQueue.pop();

				for(auto neighbor : graph->neighborsOf(vertex)) {
					if(!visited[neighbor.first]) {
						vertexQueue.push(neighbor.first);
						visited[neighbor.first] = true;
						connectedVertices++;
					}
				}
			}

			assert(connectedVertices, ==, NUMBER_OF_VERTICES);
		};
	}
} end_tests;
