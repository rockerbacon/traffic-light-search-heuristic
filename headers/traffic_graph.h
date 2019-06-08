#pragma once

#include <cstdlib>
#include <unordered_map>

namespace ufrrj {

	typedef size_t Vertice;

	class TrafficGraph {
		public:
			class Edge {
				public:
					size_t vertice1;
					size_t vertice2;

					bool operator== (const Edge& other);
			};

			virtual int weight(const Edge& edge) const = 0;
			virtual size_t getNumberOfVertices(void) const = 0;
			virtual void setTiming(Vertice vertice, int timing) = 0;
			virtual int getTiming(Vertice vertice) const = 0;
			virtual int penalty(Vertice vertice1, Vertice vertice2) const = 0;
			virtual int getCycle (void) const = 0;

	};

	class Solution {
		private:
			int* verticeTimings;
		public:
			Solution (size_t numberOfVertices);
			~Solution (void);
			void setTiming(Vertice vertice, int timing);
			int getTiming(Vertice vertice);
	};

	class AdjacencyMatrixTrafficGraph : public TrafficGraph {
		private:
			int cycle;
			int* adjacencyMatrix;
			Vertice numberOfVertices;
			Solution* solution;
		public:
			AdjacencyMatrixTrafficGraph(int* adjacencyMatrix, size_t numberOfVertices, int cycle);
			~AdjacencyMatrixTrafficGraph(void);

			virtual int weight(const Edge& edge) const;
			virtual size_t getNumberOfVertices(void) const;
			virtual void setTiming(Vertice vertice, int timing);
			virtual int getTiming(Vertice vertice) const;
			virtual int penalty(Vertice vertice1, Vertice vertice2) const;
			virtual int getCycle (void) const;

	};

	class AdjacencyListTrafficGraph : public TrafficGraph {
		private:
			size_t numberOfVertices;
			int cycle;
			std::unordered_map<Vertice, int>* adjacencyList;

		public:
			AdjacencyListTrafficGraph(std::unordered_map<Vertice, int>* adjacencyList, size_t numberOfVertices, int cycle);
			~AdjacencyListTrafficGraph(void);

			virtual int weight(const Edge& edge) const;
			virtual size_t getNumberOfVertices(void) const;
			virtual void setTiming(Vertice vertice, int timing);
			virtual int getTiming(Vertice vertice) const;
			virtual int penalty(Vertice vertice1, Vertice vertice2) const;
			virtual int getCycle (void) const;
	};

	class TrafficGraphBuilder {
		private:
			int cycle;
			std::unordered_map<Vertice, std::unordered_map<Vertice, int>*> adjacencyListMap;
			Vertice highestVerticeIndex;

		public:
			TrafficGraphBuilder();
			~TrafficGraphBuilder(void);

			void addEdge(const TrafficGraph::Edge& edge, int weight);

			TrafficGraph* buildAsAdjacencyMatrix(void);
			TrafficGraph* buildAsAdjacencyList(void);

			void withCycle(int cycle);
	};



};
