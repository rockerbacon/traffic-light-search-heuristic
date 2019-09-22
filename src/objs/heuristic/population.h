#pragma once

#include "traffic_graph/traffic_graph.h"
#include <vector>
#include <list>
#include <mutex>

namespace heuristic {

	struct Individual {
		traffic::Solution solution;
		traffic::TimeUnit penalty;
		traffic::TimeUnit minimumDistance;

		bool operator<(const Individual &other) const;
	};

	class PopulationSlice;
	class PopulationInterface {
		public:
			virtual ~PopulationInterface(void) = default;

			virtual std::vector<Individual>::iterator begin(void) = 0;
			virtual std::vector<Individual>::iterator end(void) = 0;
			virtual size_t size(void) const = 0;

			virtual Individual& operator[](size_t index) = 0;

			virtual PopulationSlice slice(size_t begin, size_t end) = 0;
	};

	class PopulationSlice : public PopulationInterface {
		private:
			std::vector<Individual>::iterator sliceBegin;
			std::vector<Individual>::iterator sliceEnd;
		public:
			PopulationSlice (void) = default;
			PopulationSlice (decltype(sliceBegin) begin, decltype(sliceEnd) end);

			decltype(sliceBegin) begin (void);
			decltype(sliceEnd) end (void);
			size_t size (void) const;

			Individual& operator[](size_t index);

			PopulationSlice slice (size_t begin, size_t end);
	};

	class Population : public PopulationInterface {
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

			PopulationSlice slice (size_t begin, size_t end);
	};


	size_t scatterSearchPopulationSize(size_t elitePopulationSize, size_t diversePopulationSize);

	struct ScatterSearchPopulation {
		PopulationSlice total;
		PopulationSlice elite;
		PopulationSlice diverse;
		PopulationSlice reference;
		PopulationSlice candidate;
		PopulationSlice battling;

		ScatterSearchPopulation (void) = default;
		ScatterSearchPopulation (PopulationInterface &population, size_t elitePopulationSize, size_t diversePopulationSize);
	};

}
