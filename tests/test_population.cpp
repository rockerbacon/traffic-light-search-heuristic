#include "assertions/assert.h"
#include "heuristic/population.h"
#include <algorithm>

using namespace heuristic;
using namespace std;
using namespace traffic;

int main (void) {

	vector<int> populationFixture = { 40, 22, 31, 20 };
	Population<int> *population;
	int defaultValue = 3;

	test_case("population instantiation raises no errors") {
		size_t numberOfIndividuals = 4;
		population = new Population<int>(numberOfIndividuals, 3);
	} end_test_case;

	test_case("population is correctly iterated with foreach") {
		size_t c = 0;
		for (auto i : *population) {
			assert_equal(i, defaultValue);
			c++;
		}
	} end_test_case;

	test_case("population is correctly iterated using iterators") {
		size_t c = 0;
		for (auto i = population->begin(); i < population->end(); i++) {
			*i = populationFixture[c];
			c++;
		}
		c = 0;
		for (auto i = population->begin(); i < population->end(); i++) {
			assert_equal(*i, populationFixture[c]);
			c++;
		}
	} end_test_case;


	test_case("population is correctly sorted") {
		sort(population->begin(), population->end());

		for (size_t i = 1; i < population->size(); i++) {
			size_t j = i-1;
			assert_less_than((*population)[j], (*population)[i]);
		}
	} end_test_case;

	test_case("population slice has correct size") {
		PopulationSlice<int> slice = population->slice(0, 2);
		assert_equal(slice.size(), 2);
	} end_test_case;

	test_case("chages in the original population affect the slice") {
		PopulationSlice<int> slice = population->slice(0, 2);
		(*population)[0]= 5;
		(*population)[1]= 2;
		assert_equal(slice[0], (*population)[0]);
		assert_equal(slice[1], (*population)[1]);
	} end_test_case;

	test_case("population slice is correctly iterated using iterators") {
		PopulationSlice<int> slice = population->slice(0, 2);
		size_t c = 0;
		for (auto i = slice.begin(); i < slice.end(); i++) {
			*i = populationFixture[c];
			c++;
		}
		assert_equal(c, 2);
		c = 0;
		for (auto i = slice.begin(); i < slice.end(); i++) {
			assert_equal(*i, populationFixture[c]);
			c++;
		}
	} end_test_case;

	test_case("population slice is correctly iterated using foreach") {
		PopulationSlice<int> slice = population->slice(0, 2);
		size_t c = 0;
		for (auto i : slice) {
			assert_equal(i, populationFixture[c]);
			c++;
		}
		assert_equal(c, 2);
	} end_test_case;

	test_case("population slice can be sliced correctly") {
		PopulationSlice<int> slice = population->slice(0, 3).slice(1, 2);
		for (size_t i = 0; i < slice.size(); i++) {
			assert_equal(slice[i], (*population)[i+1]);
		}
	} end_test_case;

	test_case("funtion to determine scatter search population size returns correct size") {
		size_t elitePopulationSize = 2;
		size_t diversePopulationSize = 6;
		size_t expectedPopulationSize = 3*(elitePopulationSize+diversePopulationSize)/2;

		assert_equal(scatterSearchPopulationSize(elitePopulationSize, diversePopulationSize), expectedPopulationSize);
	} end_test_case;

	test_case("scatter search population is created with correct sizes") {
		size_t elitePopulationSize = 1;
		size_t diversePopulationSize = 3;
		size_t referencePopulationSize = elitePopulationSize+diversePopulationSize;
		Population<int> population2(scatterSearchPopulationSize(elitePopulationSize, diversePopulationSize));
		ScatterSearchPopulation<int> scatterSearchPopulation(population2, elitePopulationSize, diversePopulationSize);

		assert_equal(scatterSearchPopulation.total.size(), 3*referencePopulationSize/2);
		assert_equal(scatterSearchPopulation.elite.size(), elitePopulationSize);
		assert_equal(scatterSearchPopulation.diverse.size(), diversePopulationSize);
		assert_equal(scatterSearchPopulation.reference.size(), referencePopulationSize);
		assert_equal(scatterSearchPopulation.candidate.size(), referencePopulationSize/2);
		assert_equal(scatterSearchPopulation.battling.size(), diversePopulationSize + referencePopulationSize/2);

	} end_test_case;

	test_case("reference population in scatter search population is the combination of elite and diverse populations") {
		ScatterSearchPopulation<int> scatterSearchPopulation(*population, 1, 3);
		assert_true(scatterSearchPopulation.elite.begin() == scatterSearchPopulation.reference.begin());
		assert_true(scatterSearchPopulation.diverse.end() == scatterSearchPopulation.reference.end());
	} end_test_case;

	test_case("elite population begins at beginning of total population") {
		ScatterSearchPopulation<int> scatterSearchPopulation(*population, 1, 3);
		assert_true(scatterSearchPopulation.elite.begin() == population->begin());
	} end_test_case;

	test_case("diverse population begins where elite popultion ends") {
		ScatterSearchPopulation<int> scatterSearchPopulation(*population, 1, 3);
		assert_true(scatterSearchPopulation.diverse.begin() == scatterSearchPopulation.elite.end());
	} end_test_case;

	test_case("candidate population begins where diverse population ends") {
		ScatterSearchPopulation<int> scatterSearchPopulation(*population, 1, 3);
		assert_true(scatterSearchPopulation.candidate.begin() == scatterSearchPopulation.diverse.end());
	} end_test_case;

	test_case("population destruction raises no errors") {
		delete population;
	} end_test_case;

}
