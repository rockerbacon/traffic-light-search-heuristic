#pragma once

#include <cstdlib>
#include <unordered_map>

namespace ufrrj {

	class TrafficGraph {
		public:
			class Edge {
				public:
					size_t vertice1;
					size_t vertice2;

					bool operator== (const Edge& other);
			};

			virtual int weight(const Edge& edge) = 0;
			virtual size_t getNumberOfVertices(void) = 0;

	};

	class AdjacencyMatrixTrafficGraph : public TrafficGraph {
		private:
			int* adjacencyMatrix;
			size_t numberOfVertices;
		public:
			AdjacencyMatrixTrafficGraph(int* adjacencyMatrix, size_t numberOfVertices);
			~AdjacencyMatrixTrafficGraph(void);

			virtual int weight(const Edge& edge);
			virtual size_t getNumberOfVertices(void);

	};

	class AdjacencyListTrafficGraph : public TrafficGraph {

	};

	class TrafficGraphBuilder {
		private:
			std::unordered_map<size_t, std::unordered_map<size_t, int>*> adjacencyListMap;
			size_t highestVerticeIndex;

		public:
			TrafficGraphBuilder();
			~TrafficGraphBuilder(void);

			void addEdge(const TrafficGraph::Edge& edge, int weight);

			TrafficGraph* buildAsAdjacencyMatrix(void);
			TrafficGraph* buildAsAdjacencyList(void);
	};



};
