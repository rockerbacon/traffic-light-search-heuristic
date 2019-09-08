#include "heuristic/heuristic.h"
#include "heuristic/population.h"
#include <vector>
#include <random>
#include <algorithm>

using namespace traffic;
using namespace std;
using namespace heuristic;

void diversify (const Graph &graph, PopulationSlice &elitePopulation, PopulationSlice &diversePopulation, PopulationSlice &candidatePopulation) {
	auto nextGenerationBegin = elitePopulation.begin();
	auto nextGenerationEnd = elitePopulation.end();
	auto battlingPopulationBegin = diversePopulation.begin();
	auto battlingPopulationEnd = candidatePopulation.end();
	TimeUnit infinity = numeric_limits<TimeUnit>::max();
	TimeUnit minusInfinity = numeric_limits<TimeUnit>::min();
	TimeUnit currentDistance;
	TimeUnit greatestMinimumDistance;
	decltype(battlingPopulationBegin) chosenIndividual;

	greatestMinimumDistance = minusInfinity;
	for (auto it = battlingPopulationBegin; it < battlingPopulationEnd; it++) {

		it->mininumDistance = infinity;
		for (auto jt = nextGenerationBegin; jt < nextGenerationEnd; jt++) {
			currentDistance = distance(graph, it->solution, jt->solution);
			if (currentDistance < it->mininumDistance) {
				it->mininumDistance = currentDistance;
			}
		}	

		if (it->mininumDistance > greatestMinimumDistance) {
			greatestMinimumDistance = it->mininumDistance;
			chosenIndividual = it;
		}
	}

	do {
		swap(*chosenIndividual, *nextGenerationEnd);
		nextGenerationEnd++;
		battlingPopulationBegin++;

		greatestMinimumDistance = minusInfinity;
		for (auto it = battlingPopulationBegin; it < battlingPopulationEnd; it++) {
			currentDistance = distance(graph, chosenIndividual->solution, it->solution);
			if (currentDistance < it->mininumDistance) {
				it->mininumDistance = currentDistance;
			}
			if (it->mininumDistance > greatestMinimumDistance) {
				greatestMinimumDistance = it->mininumDistance;
				chosenIndividual = it;
			}
		}
	} while (battlingPopulationBegin == battlingPopulationEnd);

}

Solution heuristic::scatterSearch (const Graph &graph, size_t elitePopulationSize, size_t diversePopulationSize, size_t localSearchIterations, const StopFunction &stopFunction, const CombinationMethod &combinationMethod) {

	size_t	referencePopulationSize = elitePopulationSize+diversePopulationSize,
			totalPopulationSize = referencePopulationSize + referencePopulationSize/2;

	Population population(totalPopulationSize, graph.getNumberOfVertices());

	PopulationSlice	elitePopulation(population, 0, elitePopulationSize),
				   	diversePopulation(population, elitePopulationSize, referencePopulationSize),
				   	candidatePopulation(population, referencePopulationSize, totalPopulationSize),
					referencePopulation(population, 0, referencePopulationSize);

	const Individual *individual1, *individual2;

	Metrics metrics;
	random_device seeder;
	mt19937 randomEngine(seeder());
	TimeUnit infinite = numeric_limits<TimeUnit>::max();
	StopFunction localSearchStopFunction = stop_function_factory::numberOfIterations(localSearchIterations);

	if (referencePopulationSize&1) {
		throw invalid_argument("elitePopulationSize+diversePopulationSize must be an even number");
	}

	metrics.executionBegin = chrono::high_resolution_clock::now();

	for (auto i = referencePopulation.begin(); i < referencePopulation.end(); i++) {
		Solution constructedSolution = localSearchHeuristic(graph, constructHeuristicSolution(graph), localSearchStopFunction);
		*i = {constructedSolution, graph.totalPenalty(constructedSolution)};
	}

	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	while (stopFunction(metrics)) {

		for (size_t i = 0; i < candidatePopulation.size(); i++) {

			individual1 = &referencePopulation[i*2];
			individual2 = &referencePopulation[i*2+1];

			candidatePopulation[i].solution = combinationMethod(graph, &individual1->solution, &individual2->solution);
			candidatePopulation[i].solution = localSearchHeuristic(graph, candidatePopulation[i].solution, localSearchStopFunction);
			candidatePopulation[i].penalty = graph.totalPenalty(candidatePopulation[i].solution);
		}

		sort(population.begin(), population.end());

		diversify(graph, elitePopulation, diversePopulation, candidatePopulation);
		shuffle(referencePopulation.begin(), referencePopulation.end(), randomEngine);

		metrics.numberOfIterations++;
	}
	return population[0].solution;
}
