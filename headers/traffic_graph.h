#pragma once

#include <cstdlib>
#include <unordered_map>

namespace traffic {

	typedef size_t Vertice;
	typedef unsigned int TimeUnit;
	typedef int Weight;

	class Solution {
		private:
			TimeUnit* verticeTimings;
		public:
			Solution (size_t numberOfVertices);
			~Solution (void);
			void setTiming(Vertice vertice, TimeUnit timing);
			TimeUnit getTiming(Vertice vertice);
	};

	class Graph {
		private:
			Solution* solution;
			TimeUnit cycle;
			size_t numberOfVertices;
		public:
			class Edge {
				public:
					Vertice vertice1;
					Vertice vertice2;

					bool operator== (const Edge& other) const;
			};

			Graph(size_t numberOfVertices, TimeUnit cycle);
			~Graph(void);

			virtual int weight(const Edge& edge) const = 0;
			void setTiming(Vertice vertice, TimeUnit timing);
			TimeUnit getTiming(Vertice vertice) const;
			size_t getNumberOfVertices(void) const;
			TimeUnit penalty(Vertice vertice1, Vertice vertice2) const;
			TimeUnit getCycle (void) const;

	};

	class AdjacencyMatrixGraph : public Graph {
		private:
			Weight* adjacencyMatrix;
			size_t matrixDimensionX2minus1;
		public:
			AdjacencyMatrixGraph(Weight* adjacencyMatrix, size_t numberOfVertices, TimeUnit cycle);
			~AdjacencyMatrixGraph(void);

			virtual Weight weight(const Edge& edge) const;

	};

	class AdjacencyListGraph : public Graph {
		private:
			std::unordered_map<Vertice, int>* adjacencyList;

		public:
			AdjacencyListGraph(std::unordered_map<Vertice, Weight>* adjacencyList, size_t numberOfVertices, TimeUnit cycle);
			~AdjacencyListGraph(void);

			virtual Weight weight(const Edge& edge) const;
	};

	class GraphBuilder {
		private:
			TimeUnit cycle;
			std::unordered_map<Vertice, std::unordered_map<Vertice, Weight>*> adjacencyListMap;
			Vertice highestVerticeIndex;

		public:
			GraphBuilder();
			~GraphBuilder(void);

			void addEdge(const Graph::Edge& edge, Weight weight);

			Graph* buildAsAdjacencyMatrix(void);
			Graph* buildAsAdjacencyList(void);

			void withCycle(TimeUnit cycle);
	};

};
