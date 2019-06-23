#pragma once

#include <unordered_map>
#include <iostream>

namespace traffic {

	typedef size_t Vertex;
	typedef int TimeUnit;
	typedef int Weight;

	class Solution {
		private:
			TimeUnit* vertexTimings;
			size_t numberOfVertices;
		public:
			Solution (void);
			Solution (const Solution& other);
			Solution (Solution&& other);
			explicit Solution (size_t numberOfVertices);
			~Solution (void);
			void setTiming(Vertex vertex, TimeUnit timing);
			TimeUnit getTiming(Vertex vertex) const;
			size_t getNumberOfVertices(void) const;

			friend void swap (Solution& a, Solution& b) {
				using std::swap;

				swap(a.numberOfVertices, b.numberOfVertices);
				swap(a.vertexTimings, b.vertexTimings);
			}

			Solution& operator= (Solution other);
	};

	class Graph {
		private:
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
			virtual ~Graph(void);

			virtual int weight(const Edge& edge) const = 0;
			size_t getNumberOfVertices(void) const;
			TimeUnit penalty(Vertex vertex1, Vertex vertex2, const Solution& solution, Weight weight=-1) const;
			TimeUnit getCycle (void) const;
			TimeUnit vertexPenalty(Vertex vertex, const Solution& solution) const;
			virtual const std::unordered_map<Vertex, Weight>& neighborsOf(Vertex vertex) const = 0;
			TimeUnit vertexPenaltyOnewayOnly(Vertex vertex, const Solution& solution) const;
			TimeUnit totalPenalty(const Solution& solution) const;

	};

	class AdjacencyMatrixGraph : public Graph {
		private:
			Weight* adjacencyMatrix;
			size_t matrixDimensionX2minus1;
			mutable std::unordered_map<Vertex, std::unordered_map<Vertex, Weight>*> neighborhoodRequests;
		public:
			AdjacencyMatrixGraph(Weight* adjacencyMatrix, size_t numberOfVertices, TimeUnit cycle);
			~AdjacencyMatrixGraph(void);

			Weight weight(const Edge& edge) const;
			const std::unordered_map<Vertex, Weight>& neighborsOf(Vertex vertex) const;

	};

	class AdjacencyListGraph : public Graph {
		private:
			std::unordered_map<Vertex, Weight>* adjacencyList;

		public:
			AdjacencyListGraph(std::unordered_map<Vertex, Weight>* adjacencyList, size_t numberOfVertices, TimeUnit cycle);
			~AdjacencyListGraph(void);

			Weight weight(const Edge& edge) const;
			const std::unordered_map<Vertex, Weight>& neighborsOf(Vertex vertex) const;
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
