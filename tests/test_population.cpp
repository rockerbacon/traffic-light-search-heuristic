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
	PopulationSlice *populationSlice;
	Solution tmpSolution(3);

	tmpSolution.setTiming(0, 2);
	tmpSolution.setTiming(1, 4);
	tmpSolution.setTiming(2, 6);
	populationFixture.push_back({tmpSolution, 40});

	tmpSolution.setTiming(0, 6);
	tmpSolution.setTiming(1, 3);
	tmpSolution.setTiming(2, 8);
	populationFixture.push_back({tmpSolution, 22});

	tmpSolution.setTiming(0, 5);
	tmpSolution.setTiming(1, 7);
	tmpSolution.setTiming(2, 0);
	populationFixture.push_back({tmpSolution, 31});

	test_case("population instantiation raises no errors") {
		size_t numberOfIndividuals = 3;
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

	test_case("population slice instantiation raises no errors") {
		size_t begin = 0;
		size_t end = 2;
		populationSlice = new PopulationSlice(*population, begin, end);
	} end_test_case;

	test_case("population slice has correct size") {
		assert_equal(populationSlice->size(), 2);
	} end_test_case;

	test_case("chages in the original population affect the slice") {
		(*population)[0].penalty = 5;
		(*population)[0].solution = Solution(3);
		(*population)[1].penalty = 2;
		(*population)[1].solution = Solution(3);
		(*population)[1].solution.setTiming(1, 11);
		customAssertEqual((*populationSlice)[0], (*population)[0]);
		customAssertEqual((*populationSlice)[1], (*population)[1]);
	} end_test_case;

	test_case("population slice is correctly iterated using iterators") {
		size_t c = 0;
		for (auto i = populationSlice->begin(); i < populationSlice->end(); i++) {
			*i = populationFixture[c];
			c++;
		}
		assert_equal(c, 2);
		c = 0;
		for (auto i = populationSlice->begin(); i < populationSlice->end(); i++) {
			customAssertEqual(*i, populationFixture[c]);
			c++;
		}
	} end_test_case;

	test_case("population slice is correctly iterated using foreach") {
		size_t c = 0;
		for (Individual i : *populationSlice) {
			customAssertEqual(i, populationFixture[c]);
			c++;
		}
		assert_equal(c, 2);
	} end_test_case;

	test_case("population destruction raises no errors") {
		delete population;
	} end_test_case;

}
