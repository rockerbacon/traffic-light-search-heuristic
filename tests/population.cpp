#include <assertions-test/test.h>
#include <heuristic/population.h>
#include <algorithm>

using namespace heuristic;
using namespace std;
using namespace traffic;

Population<int> populationFixture() {
	Population<int> population(4, 0);
	auto i = 0;
	for (auto individual : {40, 22, 31, 20}) {
		population[i++] = individual;
	}
	return population;
}

begin_tests {

	Population<int> *population;
	int defaultValue = 3;

	test_suite("when iterating through population") {
		test_case("population should be correctly iterated with foreach") {
			int defaultIndividual = 3;
			Population<int> population(4, defaultIndividual);

			auto populationSize = 0;
			for (auto i : population) {
				assert(i, ==, defaultIndividual);
				populationSize++;
			}
			assert(populationSize, ==, 4);
		};

		test_case("population should be correctly iterated using iterators") {
			int defaultIndividual = 3;
			Population<int> population(4, defaultIndividual);

			size_t populationSize = 0;
			for (auto i = population.begin(); i < population.end(); i++) {
				assert(*i, ==, defaultIndividual);
				populationSize++;
			}
			assert(populationSize, ==, 4);
		};


		test_case("population should be correctly sorted") {
			auto population = populationFixture();

			sort(population.begin(), population.end());

			for (size_t i = 1; i < population.size(); i++) {
				size_t j = i-1;
				assert(population[j], <, population[i]);
			}
		};
	}

	test_suite("when slicing the population") {
		test_case("population slice should have the correct size") {
			auto population = populationFixture();
			PopulationSlice<int> slice = population.slice(0, 2);
			assert(slice.size(), ==, 2);
		};

		test_case("changes in the original population should affect the slice") {
			auto population = populationFixture();
			PopulationSlice<int> slice = population.slice(0, 2);
			population[0]= 5;
			population[1]= 2;
			assert(slice[0], ==, population[0]);
			assert(slice[1], ==, population[1]);
		};

		test_case("population slice should be correctly iterated using iterators") {
			auto population = populationFixture();
			PopulationSlice<int> slice = population.slice(0, 2);
			size_t populationSize = 0;
			for (auto i = slice.begin(); i < slice.end(); i++) {
				assert(*i, ==, population[populationSize]);
				populationSize++;
			}
			assert(populationSize, ==, 2);
		};

		test_case("population slice should be correctly iterated using foreach") {
			auto population = populationFixture();
			PopulationSlice<int> slice = population.slice(0, 2);
			size_t populationSize = 0;
			for (auto i : slice) {
				assert(i, ==, population[populationSize]);
				populationSize++;
			}
			assert(populationSize, ==, 2);
		};

		test_case("population slice can be sliced further correctly") {
			auto population = populationFixture();
			PopulationSlice<int> slice = population.slice(0, 3).slice(1, 2);
			for (size_t i = 0; i < slice.size(); i++) {
				assert(slice[i], ==, population[i+1]);
			}
		};
	}


	test_suite("when preparing scatter search population") {
		test_case("should determine correct total population size") {
			size_t elitePopulationSize = 2;
			size_t diversePopulationSize = 6;
			size_t expectedPopulationSize = 3*(elitePopulationSize+diversePopulationSize)/2;

			assert(scatterSearchPopulationSize(elitePopulationSize, diversePopulationSize), ==, expectedPopulationSize);
		};

		test_case("should create population with correct subpopulation sizes") {
			size_t elitePopulationSize = 1;
			size_t diversePopulationSize = 3;
			size_t referencePopulationSize = elitePopulationSize+diversePopulationSize;
			Population<int> population(scatterSearchPopulationSize(elitePopulationSize, diversePopulationSize));
			ScatterSearchPopulation<int> scatterSearchPopulation(population, elitePopulationSize, diversePopulationSize);

			assert(scatterSearchPopulation.total.size(), ==, 3*referencePopulationSize/2);
			assert(scatterSearchPopulation.elite.size(), ==, elitePopulationSize);
			assert(scatterSearchPopulation.diverse.size(), ==, diversePopulationSize);
			assert(scatterSearchPopulation.reference.size(), ==, referencePopulationSize);
			assert(scatterSearchPopulation.candidate.size(), ==, referencePopulationSize/2);
			assert(scatterSearchPopulation.battling.size(), ==, diversePopulationSize + referencePopulationSize/2);
		};

		test_case("reference population should be the combination of elite and diverse populations") {
			auto population = populationFixture();
			ScatterSearchPopulation<int> scatterSearchPopulation(population, 1, 3);
			assert(scatterSearchPopulation.elite.begin(), ==, scatterSearchPopulation.reference.begin());
			assert(scatterSearchPopulation.diverse.end(), ==, scatterSearchPopulation.reference.end());
		};

		test_case("elite population should begin at beginning of total population") {
			auto population = populationFixture();
			ScatterSearchPopulation<int> scatterSearchPopulation(population, 1, 3);
			assert(scatterSearchPopulation.elite.begin(), ==, population.begin());
		};

		test_case("diverse population should begin where elite popultion ends") {
			auto population = populationFixture();
			ScatterSearchPopulation<int> scatterSearchPopulation(population, 1, 3);
			assert(scatterSearchPopulation.diverse.begin(), ==, scatterSearchPopulation.elite.end());
		};

		test_case("candidate population should begin where diverse population ends") {
			auto population = populationFixture();
			ScatterSearchPopulation<int> scatterSearchPopulation(population, 1, 3);
			assert(scatterSearchPopulation.candidate.begin(), ==, scatterSearchPopulation.diverse.end());
		};
	}
} end_tests;
