#pragma once

#include <unordered_map>

namespace traffic {

	typedef size_t Vertex;
	typedef unsigned int TimeUnit;
	typedef int Weight;

	class Solution {
		private:
			TimeUnit* vertexTimings;
		public:
			Solution (size_t numberOfVertices);
			~Solution (void);
			void setTiming(Vertex vertex, TimeUnit timing);
			TimeUnit getTiming(Vertex vertex);
	};

	class Graph {
		private:
			Solution* solution;
			TimeUnit cycle;
			size_t numberOfVertices;
		public:
			class Edge {
				public:
					Vertex vertex1;
					Vertex vertex2;

					bool operator== (const Edge& other) const;
			};

			Graph(size_t numberOfVertices, TimeUnit cycle);
			~Graph(void);

			virtual int weight(const Edge& edge) const = 0;
			void setTiming(Vertex vertex, TimeUnit timing);
			TimeUnit getTiming(Vertex vertex) const;
			size_t getNumberOfVertices(void) const;
			TimeUnit penalty(Vertex vertex1, Vertex vertex2, Weight weight=-1) const;
			TimeUnit getCycle (void) const;
			TimeUnit vertexPenalty(Vertex vertex) const;
			virtual const std::unordered_map<Vertex, Weight>& neighborsOf(Vertex vertex) const = 0;
			TimeUnit vertexPenaltyOnewayOnly(Vertex vertex) const;

	};

	class AdjacencyMatrixGraph : public Graph {
		private:
			Weight* adjacencyMatrix;
			size_t matrixDimensionX2minus1;
			mutable std::unordered_map<Vertex, std::unordered_map<Vertex, Weight>*> neighborhoodRequests;
		public:
			AdjacencyMatrixGraph(Weight* adjacencyMatrix, size_t numberOfVertices, TimeUnit cycle);
			~AdjacencyMatrixGraph(void);

			virtual Weight weight(const Edge& edge) const;
			virtual const std::unordered_map<Vertex, Weight>& neighborsOf(Vertex vertex) const;

	};

	class AdjacencyListGraph : public Graph {
		private:
			std::unordered_map<Vertex, Weight>* adjacencyList;

		public:
			AdjacencyListGraph(std::unordered_map<Vertex, Weight>* adjacencyList, size_t numberOfVertices, TimeUnit cycle);
			~AdjacencyListGraph(void);

			virtual Weight weight(const Edge& edge) const;
			virtual const std::unordered_map<Vertex, Weight>& neighborsOf(Vertex vertex) const;
	};

	class GraphBuilder {
		private:
			TimeUnit cycle;
			std::unordered_map<Vertex, std::unordered_map<Vertex, Weight>*> adjacencyListMap;
			Vertex highestVertexIndex;

		public:
			GraphBuilder(void);
			GraphBuilder(size_t nVertices, unsigned minDegree, unsigned maxDegree, int minWeight, int maxWeight);
			~GraphBuilder(void);

			bool addEdge(const Graph::Edge& edge, Weight weight);

			AdjacencyMatrixGraph* buildAsAdjacencyMatrix(void) const;
			AdjacencyListGraph* buildAsAdjacencyList(void) const;

			void withCycle(TimeUnit cycle);
	};

};
