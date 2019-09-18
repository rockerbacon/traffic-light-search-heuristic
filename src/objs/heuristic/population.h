#pragma once

#include "traffic_graph/traffic_graph.h"
#include <vector>

namespace heuristic {

	struct Individual {
		traffic::Solution solution;
		traffic::TimeUnit penalty;
		traffic::TimeUnit minimumDistance;

		bool operator<(const Individual &other) const;
	};

	class Population {
		private:
			std::vector<Individual> individuals;
		public:
			Population (size_t numberOfIndividuals, traffic::Vertex individualsSize);

			decltype(individuals)::iterator begin (void);
			decltype(individuals)::iterator end (void);
			decltype(individuals)::const_iterator cbegin (void) const;
			decltype(individuals)::const_iterator cend (void) const;
			size_t size (void) const;

			const Individual& operator[](size_t index) const;
			Individual& operator[](size_t index);
	};

	class PopulationSlice {
		private:
			std::vector<Individual>::iterator sliceBegin;
			std::vector<Individual>::iterator sliceEnd;
		public:
			PopulationSlice (void) = default;
			PopulationSlice (Population &population, size_t begin, size_t end);
			PopulationSlice (decltype(sliceBegin) begin, decltype(sliceEnd) end);
			PopulationSlice (PopulationSlice &population, size_t begin, size_t end);

			decltype(sliceBegin) begin (void);
			decltype(sliceEnd) end (void);
			size_t size (void) const;

			Individual& operator[](size_t index);
	};

	size_t scatterSearchPopulationSize(size_t elitePopulationSize, size_t diversePopulationSize);

	struct ScatterSearchPopulation {
		PopulationSlice total;
		PopulationSlice elite;
		PopulationSlice diverse;
		PopulationSlice reference;
		PopulationSlice candidate;

		ScatterSearchPopulation (Population &population, size_t elitePopulationSize, size_t diversePopulationSize);
	};

}
