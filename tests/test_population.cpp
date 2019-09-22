#include "assertions/assert.h"
#include "heuristic/population.h"
#include <algorithm>

using namespace heuristic;
using namespace std;
using namespace traffic;

void customAssertEqual(const Solution &s1, const Solution &s2) {
	assert_equal(s1.getNumberOfVertices(), s2.getNumberOfVertices()); 
	for (Vertex v = 0; v < s1.getNumberOfVertices(); v++) {
		assert_equal(s1.getTiming(v), s2.getTiming(v));
	}
}

void customAssertEqual(const Individual &i1, const Individual &i2) {
	assert_equal(i1.penalty, i2.penalty);
   	customAssertEqual(i1.solution, i2.solution);
}

int main (void) {

	vector<Individual> populationFixture;
	Population *population;
	Solution tmpSolution(3);

	tmpSolution.setTiming(0, 2);
	tmpSolution.setTiming(1, 4);
	tmpSolution.setTiming(2, 6);
	populationFixture.push_back({tmpSolution, 40, 0});

	tmpSolution.setTiming(0, 6);
	tmpSolution.setTiming(1, 3);
	tmpSolution.setTiming(2, 8);
	populationFixture.push_back({tmpSolution, 22, 0});

	tmpSolution.setTiming(0, 5);
	tmpSolution.setTiming(1, 7);
	tmpSolution.setTiming(2, 0);
	populationFixture.push_back({tmpSolution, 31, 0});

	tmpSolution.setTiming(0, 1);
	tmpSolution.setTiming(1, 5);
	tmpSolution.setTiming(2, 1);
	populationFixture.push_back({tmpSolution, 20, 0});

	test_case("population instantiation raises no errors") {
		size_t numberOfIndividuals = 4;
		Vertex individualsSize = 3;
		population = new Population(numberOfIndividuals, individualsSize);
	} end_test_case;

	test_case("population is correctly iterated using iterators") {
		size_t c = 0;
		for (auto i = population->begin(); i < population->end(); i++) {
			*i = populationFixture[c];
			c++;
		}
		c = 0;
		for (auto i = population->begin(); i < population->end(); i++) {
			customAssertEqual(*i, populationFixture[c]);
			c++;
		}
	} end_test_case;

	test_case("population is correctly iterated with foreach") {
		size_t c = 0;
		for (Individual i : *population) {
			customAssertEqual(i, populationFixture[c]);
			c++;
		}
	} end_test_case;

	test_case("population is correctly sorted") {
		sort(population->begin(), population->end());

		for (size_t i = 1; i < population->size(); i++) {
			size_t j = i-1;
			assert_less_than((*population)[j].penalty, (*population)[i].penalty);
		}
	} end_test_case;

	test_case("population slice has correct size") {
		PopulationSlice slice = population->slice(0, 2);
		assert_equal(slice.size(), 2);
	} end_test_case;

	test_case("chages in the original population affect the slice") {
		PopulationSlice slice = population->slice(0, 2);
		(*population)[0].penalty = 5;
		(*population)[0].solution = Solution(3);
		(*population)[1].penalty = 2;
		(*population)[1].solution = Solution(3);
		(*population)[1].solution.setTiming(1, 11);
		customAssertEqual(slice[0], (*population)[0]);
		customAssertEqual(slice[1], (*population)[1]);
	} end_test_case;

	test_case("population slice is correctly iterated using iterators") {
		PopulationSlice slice = population->slice(0, 2);
		size_t c = 0;
		for (auto i = slice.begin(); i < slice.end(); i++) {
			*i = populationFixture[c];
			c++;
		}
		assert_equal(c, 2);
		c = 0;
		for (auto i = slice.begin(); i < slice.end(); i++) {
			customAssertEqual(*i, populationFixture[c]);
			c++;
		}
	} end_test_case;

	test_case("population slice is correctly iterated using foreach") {
		PopulationSlice slice = population->slice(0, 2);
		size_t c = 0;
		for (Individual i : slice) {
			customAssertEqual(i, populationFixture[c]);
			c++;
		}
		assert_equal(c, 2);
	} end_test_case;

	test_case("population slice can be sliced correctly") {
		PopulationSlice slice = population->slice(0, 3).slice(1, 2);
		for (size_t i = 0; i < slice.size(); i++) {
			customAssertEqual(slice[i], (*population)[i+1]);
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
		Population population2(scatterSearchPopulationSize(elitePopulationSize, diversePopulationSize), 3);
		ScatterSearchPopulation scatterSearchPopulation(population2, elitePopulationSize, diversePopulationSize);

		assert_equal(scatterSearchPopulation.total.size(), 3*referencePopulationSize/2);
		assert_equal(scatterSearchPopulation.elite.size(), elitePopulationSize);
		assert_equal(scatterSearchPopulation.diverse.size(), diversePopulationSize);
		assert_equal(scatterSearchPopulation.reference.size(), referencePopulationSize);
		assert_equal(scatterSearchPopulation.candidate.size(), referencePopulationSize/2);
		assert_equal(scatterSearchPopulation.battling.size(), diversePopulationSize + referencePopulationSize/2);

	} end_test_case;

	test_case("reference population in scatter search population is the combination of elite and diverse populations") {
		ScatterSearchPopulation scatterSearchPopulation(*population, 1, 3);
		assert_true(scatterSearchPopulation.elite.begin() == scatterSearchPopulation.reference.begin());
		assert_true(scatterSearchPopulation.diverse.end() == scatterSearchPopulation.reference.end());
	} end_test_case;

	test_case("elite population begins at beginning of total population") {
		ScatterSearchPopulation scatterSearchPopulation(*population, 1, 3);
		assert_true(scatterSearchPopulation.elite.begin() == population->begin());
	} end_test_case;

	test_case("diverse population begins where elite popultion ends") {
		ScatterSearchPopulation scatterSearchPopulation(*population, 1, 3);
		assert_true(scatterSearchPopulation.diverse.begin() == scatterSearchPopulation.elite.end());
	} end_test_case;

	test_case("candidate population begins where diverse population ends") {
		ScatterSearchPopulation scatterSearchPopulation(*population, 1, 3);
		assert_true(scatterSearchPopulation.candidate.begin() == scatterSearchPopulation.diverse.end());
	} end_test_case;

	test_case("population destruction raises no errors") {
		delete population;
	} end_test_case;

}
