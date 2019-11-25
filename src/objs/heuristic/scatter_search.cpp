#include "heuristic.h"
#include <vector>
#include <random>
#include <algorithm>

using namespace traffic;
using namespace std;
using namespace heuristic;

TimeUnit heuristic::diversify (const Graph &graph, ScatterSearchPopulation<Individual*> &population) {
	auto nextGenerationBegin = population.elite.begin();
	auto nextGenerationEnd = population.elite.end();
	auto battlingPopulationBegin = population.diverse.begin();
	auto battlingPopulationEnd = population.candidate.end();
	TimeUnit infinity = numeric_limits<TimeUnit>::max();
	TimeUnit minusInfinity = numeric_limits<TimeUnit>::min();
	TimeUnit currentDistance;
	TimeUnit greatestMinimumDistance;
	TimeUnit lowestDistance = numeric_limits<TimeUnit>::max();
	decltype(battlingPopulationBegin) chosenIndividual;
	decltype(battlingPopulationBegin) nextChosenIndividual;

	greatestMinimumDistance = minusInfinity;
	for (auto it = battlingPopulationBegin; it != battlingPopulationEnd; it++) {

		(*it)->minimumDistance = infinity;
		for (auto jt = nextGenerationBegin; jt < nextGenerationEnd; jt++) {
			currentDistance = distance(graph, (*it)->solution, (*jt)->solution);
			if (currentDistance < lowestDistance) {
				lowestDistance = currentDistance;
			}
			if (currentDistance < (*it)->minimumDistance) {
				(*it)->minimumDistance = currentDistance;
			}
		}

		if ((*it)->minimumDistance > greatestMinimumDistance) {
			greatestMinimumDistance = (*it)->minimumDistance;
			chosenIndividual = it;
		}
	}

	swap(*chosenIndividual, *nextGenerationEnd);
	nextGenerationEnd++;
	battlingPopulationBegin++;

	while (nextGenerationEnd != population.diverse.end()) {

		greatestMinimumDistance = minusInfinity;
		for (auto it = battlingPopulationBegin; it != battlingPopulationEnd; it++) {
			currentDistance = distance(graph, (*chosenIndividual)->solution, (*it)->solution);
			if (currentDistance < lowestDistance) {
				lowestDistance = currentDistance;
			}
			if (currentDistance < (*it)->minimumDistance) {
				(*it)->minimumDistance = currentDistance;
			}
			if ((*it)->minimumDistance > greatestMinimumDistance) {
				greatestMinimumDistance = (*it)->minimumDistance;
				nextChosenIndividual = it;
			}
		}

		chosenIndividual = nextChosenIndividual;

		swap(*chosenIndividual, *nextGenerationEnd);
		nextGenerationEnd++;
		battlingPopulationBegin++;

	}

	return lowestDistance;
}

Solution heuristic::scatterSearch (const Graph &graph, size_t elitePopulationSize, size_t diversePopulationSize, size_t localSearchIterations, const StopFunction &stopFunction, const CombinationMethod &combinationMethod) {

	size_t	referencePopulationSize = elitePopulationSize+diversePopulationSize,
			totalPopulationSize = referencePopulationSize + referencePopulationSize/2;

	Population<Individual> totalPopulation(totalPopulationSize, graph.getNumberOfVertices());
	Population<Individual*> totalPopulationReferences(totalPopulation.size());
	for (size_t i = 0; i < totalPopulation.size(); i++) {
		totalPopulationReferences[i] = &totalPopulation[i];
	}

	ScatterSearchPopulation<Individual*> population = ScatterSearchPopulation<Individual*>(totalPopulationReferences, elitePopulationSize, diversePopulationSize);

	const Individual *individual1, *individual2;

	Metrics metrics;
	random_device seeder;
	mt19937 randomEngine(seeder());
	StopFunction diverseLocalSearchStopFunction = stop_function_factory::numberOfIterations(localSearchIterations);
	StopFunction eliteLocalSearchStopFunction = stop_function_factory::numberOfIterations(localSearchIterations*10);

	if (referencePopulationSize&1) {
		throw invalid_argument("elitePopulationSize+diversePopulationSize must be an even number");
	}

	metrics.executionBegin = chrono::high_resolution_clock::now();

	for (auto i = population.elite.begin(); i < population.elite.end(); i++) {
		Solution constructedSolution = localSearchHeuristic(graph, constructHeuristicSolution(graph), eliteLocalSearchStopFunction);
		**i = {constructedSolution, graph.totalPenalty(constructedSolution), 0};
	}

	for (auto i = population.diverse.begin(); i < population.diverse.end(); i++) {
		Solution constructedSolution = localSearchHeuristic(graph, constructHeuristicSolution(graph), diverseLocalSearchStopFunction);
		**i = {constructedSolution, graph.totalPenalty(constructedSolution), 0};
	}

	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	metrics.penalty = numeric_limits<TimeUnit>::max();
	while (stopFunction(metrics)) {

		shuffle(population.reference.begin(), population.reference.end(), randomEngine);
		for (size_t i = 0; i < population.candidate.size(); i++) {

			individual1 = population.reference[i*2];
			individual2 = population.reference[i*2+1];

			population.candidate[i]->solution = combinationMethod(graph, individual1->solution, individual2->solution);
			population.candidate[i]->solution = localSearchHeuristic(graph, population.candidate[i]->solution, diverseLocalSearchStopFunction);
			population.candidate[i]->penalty = graph.totalPenalty(population.candidate[i]->solution);
		}

		sort(population.total.begin(), population.total.end(), [](auto a, auto b) { return a->penalty < b->penalty; });

		metrics.penalty = population.elite[0]->penalty;

		diversify(graph, population);

		metrics.numberOfIterations++;
	}
	return population.elite[0]->solution;
}
