#pragma once

#include "../traffic_graph/traffic_graph.h"
#include <vector>
#include <list>
#include <mutex>

namespace heuristic {

	struct Individual {
		traffic::Solution solution;
		traffic::TimeUnit penalty;
		traffic::TimeUnit minimumDistance;
	};

	template<typename T>
	class PopulationSlice;

	template<typename T>
	class PopulationInterface {
		public:
			typedef typename std::vector<T>::iterator iterator;
			virtual ~PopulationInterface(void) = default;

			virtual iterator begin(void) = 0;
			virtual iterator end(void) = 0;
			virtual size_t size(void) const = 0;

			virtual T& operator[](size_t index) = 0;
			virtual const T& operator[](size_t index) const = 0;

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
			const T& operator[](size_t index) const {
				decltype(PopulationSlice::sliceBegin) iteratorToIndex = this->sliceBegin;
				advance(iteratorToIndex, index);
				return *iteratorToIndex;
			}

			PopulationSlice<T> slice (size_t begin, size_t end) {
				return PopulationSlice(this->begin()+begin, this->begin()+end);
			}
	};

	template<typename T>
	class Population : public PopulationInterface<T>, public std::vector<T> {
		public:
			Population() = default;
			Population(size_t size) :
				std::vector<T>(size)
			{}
			Population(size_t size, const T& prototype) :
				std::vector<T>(size, prototype)
			{}
			Population(const typename std::vector<T>::iterator& rangeBegin, const typename std::vector<T>::iterator& rangeEnd) :
				std::vector<T>(rangeBegin, rangeEnd)
			{}
			typename PopulationInterface<T>::iterator begin() {
				return std::vector<T>::begin();
			}
			typename PopulationInterface<T>::iterator end() {
				return std::vector<T>::end();
			}
			size_t size() const {
				return std::vector<T>::size();
			}
			T& operator[](size_t index) {
				return std::vector<T>::operator[](index);
			}
			const T& operator[](size_t index) const {
				return std::vector<T>::operator[](index);
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
