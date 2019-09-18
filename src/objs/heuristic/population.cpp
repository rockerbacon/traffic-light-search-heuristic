#include "heuristic/population.h"

using namespace std;
using namespace heuristic;
using namespace traffic;

bool Individual::operator<(const Individual &other) const {
	return this->penalty < other.penalty;
}

Population::Population(size_t numberOfIndividuals, Vertex individualsSize) {
	this->individuals.reserve(numberOfIndividuals);
	for (size_t i = 0; i < numberOfIndividuals; i++) {
		this->individuals.push_back({
			Solution(individualsSize),
			-1,
			-1
		});
	}
}

decltype(Population::individuals)::iterator Population::begin (void) {
	return this->individuals.begin();
}

decltype(Population::individuals)::iterator Population::end (void) {
	return this->individuals.end();
}

decltype(Population::individuals)::const_iterator Population::cbegin (void) const {
	return this->individuals.cbegin();
}

decltype(Population::individuals)::const_iterator Population::cend (void) const {
	return this->individuals.cend();
}

size_t Population::size (void) const {
	return this->individuals.size();
}

const Individual& Population::operator[](size_t index) const {
	return this->individuals[index];
}

Individual& Population::operator[] (size_t index) {
	return this->individuals[index];
}

ScatterSearchPopulation::ScatterSearchPopulation (Population &population, size_t elitePopulationSize, size_t diversePopulationSize) :
	total(population, 0, population.size()),
	elite(total, 0, elitePopulationSize),
	diverse(elite.end(), elite.end()+diversePopulationSize),
	reference(elite.begin(), diverse.end()),
	candidate(reference.end(), reference.end()+reference.size()/2) 
{}	
