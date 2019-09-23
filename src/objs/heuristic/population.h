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

		Individual(const traffic::Solution &solution, traffic::TimeUnit penalty, traffic::TimeUnit minimumDistance);
		Individual(traffic::Vertex numberOfVertices);
		bool operator<(const Individual &other) const;
	};

	template<typename T>
	class PopulationSlice;

	template<typename T>
	class PopulationInterface {
		public:
			virtual ~PopulationInterface(void) = default;

			virtual typename std::vector<T>::iterator begin(void) = 0;
			virtual typename std::vector<T>::iterator end(void) = 0;
			virtual size_t size(void) const = 0;

			virtual T& operator[](size_t index) = 0;

			virtual PopulationSlice<T> slice(size_t begin, size_t end) = 0;
	};

	template<typename T>
	class PopulationSlice : public PopulationInterface<T> {
		private:
			typename std::vector<T>::iterator sliceBegin;
			typename std::vector<T>::iterator sliceEnd;
		public:
			PopulationSlice (void) = default;
			PopulationSlice (decltype(sliceBegin) begin, decltype(sliceEnd) end) :
				sliceBegin(begin),
				sliceEnd(end)
			{}

			decltype(sliceBegin) begin (void) {
				return this->sliceBegin;
			}
			decltype(sliceEnd) end (void) {
				return this->sliceEnd;
			}
			size_t size (void) const {
				return this->sliceEnd - this->sliceBegin;
			}

			T& operator[](size_t index) {
				decltype(PopulationSlice::sliceBegin) iteratorToIndex = this->begin();
				advance(iteratorToIndex, index);
				return *iteratorToIndex;
			}

			PopulationSlice<T> slice (size_t begin, size_t end) {
				return PopulationSlice(this->begin()+begin, this->begin()+end);
			}
	};

	template<typename T>
	class Population : public PopulationInterface<T> {
		private:
			std::vector<T> individuals;
		public:
			template<typename... Varargs>
			Population (size_t numberOfIndividuals, Varargs... constructor_args) {
				this->individuals.reserve(numberOfIndividuals);
				for (size_t i = 0; i < numberOfIndividuals; i++) {
					this->individuals.push_back(T(constructor_args...));
				}
			}

			typename decltype(individuals)::iterator begin (void) {
				return this->individuals.begin();
			}
			typename decltype(individuals)::iterator end (void) {
				return this->individuals.end();
			}
			typename decltype(individuals)::const_iterator cbegin (void) const {
				return this->individuals.cbegin();
			}
			typename decltype(individuals)::const_iterator cend (void) const {
				return this->individuals.cend();
			}
			size_t size (void) const {
				return this->individuals.size();
			}

			const T& operator[](size_t index) const {
				return this->individuals[index];
			}
			T& operator[](size_t index) {
				return this->individuals[index];
			}

			PopulationSlice<T> slice (size_t begin, size_t end) {
				return PopulationSlice<T>(this->begin()+begin, this->begin()+end);
			}
	};

	size_t scatterSearchPopulationSize(size_t elitePopulationSize, size_t diversePopulationSize);

	template<typename T>
	struct ScatterSearchPopulation {
		PopulationSlice<T> total;
		PopulationSlice<T> elite;
		PopulationSlice<T> diverse;
		PopulationSlice<T> reference;
		PopulationSlice<T> candidate;
		PopulationSlice<T> battling;

		ScatterSearchPopulation (void) = default;
		ScatterSearchPopulation (PopulationInterface<T> &population, size_t elitePopulationSize, size_t diversePopulationSize) :
			total(population.begin(), population.end()),
			elite(population.begin(), population.begin()+elitePopulationSize),
			diverse(elite.end(), elite.end()+diversePopulationSize),
			reference(elite.begin(), diverse.end()),
			candidate(reference.end(), reference.end()+reference.size()/2),
			battling(diverse.begin(), candidate.end())
		{}
	};

}
