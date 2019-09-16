#include "heuristic/population.h"
#include <iterator>

using namespace heuristic;
using namespace std;

PopulationSlice::PopulationSlice(Population &population, size_t begin, size_t end) {
	this->sliceBegin = population.begin()+begin;
	this->sliceEnd = population.begin()+end;
}

PopulationSlice::PopulationSlice(decltype(PopulationSlice::sliceBegin) begin, decltype(PopulationSlice::sliceEnd) end) :
	sliceBegin(begin),
	sliceEnd(end)
{}

PopulationSlice::PopulationSlice(PopulationSlice &population, size_t begin, size_t end) {
	this->sliceBegin = population.begin()+begin;
	this->sliceEnd = population.begin()+end;
}

decltype(PopulationSlice::sliceBegin) PopulationSlice::begin (void) {
	return this->sliceBegin;
}

decltype(PopulationSlice::sliceEnd) PopulationSlice::end (void) {
	return this->sliceEnd;
}

size_t PopulationSlice::size (void) const {
	return this->sliceEnd - this->sliceBegin;
}

Individual& PopulationSlice::operator[](size_t index) {
	decltype(PopulationSlice::sliceBegin) iteratorToIndex = this->begin();
	advance(iteratorToIndex, index);
	return *iteratorToIndex;
}
