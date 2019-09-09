#include "heuristic/heuristic.h"
#include "heuristic/population.h"
#include "parallel/parallel.h"
#include <vector>
#include <random>
#include <algorithm>
#include <thread>

using namespace traffic;
using namespace std;
using namespace heuristic;

void diversify (const Graph &graph, PopulationSlice &elitePopulation, PopulationSlice &diversePopulation, PopulationSlice &candidatePopulation, unsigned numberOfThreads) {
	auto nextGenerationBegin = elitePopulation.begin();
	auto nextGenerationEnd = elitePopulation.end();
	auto battlingPopulationBegin = diversePopulation.begin();
	auto battlingPopulationEnd = candidatePopulation.end();
	TimeUnit infinity = numeric_limits<TimeUnit>::max();
	TimeUnit minusInfinity = numeric_limits<TimeUnit>::min();
	TimeUnit greatestMinimumDistance;
	TimeUnit *greatestMinimumDistanceInThread;
	decltype(battlingPopulationBegin) chosenIndividual;
	decltype(chosenIndividual) *individualChosenByThread;
	unsigned threadsUsed;

	greatestMinimumDistanceInThread = new TimeUnit[numberOfThreads];
	individualChosenByThread = new decltype(chosenIndividual)[numberOfThreads];

	for (auto i = 0; i < numberOfThreads; i++) {
		greatestMinimumDistanceInThread[i] = minusInfinity;
	}
	parallel_for (battlingPopulationBegin, battlingPopulationEnd, numberOfThreads) {
		TimeUnit currentDistance;

		i->mininumDistance = infinity;
		for (auto j = nextGenerationBegin; j < nextGenerationEnd; j++) {
			currentDistance = distance(graph, i->solution, j->solution);
			if (currentDistance < i->mininumDistance) {
				i->mininumDistance = currentDistance;
			}
		}	

		if (i->mininumDistance > greatestMinimumDistanceInThread[thread_i]) {
			greatestMinimumDistanceInThread[thread_i] = i->mininumDistance;
			individualChosenByThread[thread_i] = i;
		}

	} end_parallel_for;

	chosenIndividual = individualChosenByThread[0];
	for (auto i = 1; i < numberOfThreads; i++) {
		if (individualChosenByThread[i]->mininumDistance > chosenIndividual->mininumDistance) {
			chosenIndividual = individualChosenByThread[i];
		}
	}

	swap(*chosenIndividual, *nextGenerationEnd);
	nextGenerationEnd++;
	battlingPopulationBegin++;

	while (nextGenerationEnd != diversePopulation.end()) {

		for (auto i = 0; i < numberOfThreads; i++) {
			greatestMinimumDistanceInThread[i] = minusInfinity;
		}
		parallel_for (battlingPopulationBegin, battlingPopulationEnd, numberOfThreads) {
			auto currentDistance = distance(graph, chosenIndividual->solution, i->solution);
			if (currentDistance < i->mininumDistance) {
				i->mininumDistance = currentDistance;
			}
			if (i->mininumDistance > greatestMinimumDistanceInThread[thread_i]) {
				greatestMinimumDistanceInThread[thread_i] = i->mininumDistance;
				individualChosenByThread[thread_i] = i;
			}
		} end_parallel_for;

		threadsUsed = ::parallel::usable_threads(battlingPopulationEnd-battlingPopulationBegin, numberOfThreads);
		chosenIndividual = individualChosenByThread[0];
		for (auto i = 1; i < threadsUsed; i++) {
			if (individualChosenByThread[i]->mininumDistance > chosenIndividual->mininumDistance) {
				chosenIndividual = individualChosenByThread[i];
			}
		}

		swap(*chosenIndividual, *nextGenerationEnd);
		nextGenerationEnd++;
		battlingPopulationBegin++;

	}

}


Solution heuristic::parallel::scatterSearch (const Graph &graph, size_t elitePopulationSize, size_t diversePopulationSize, size_t localSearchIterations, const StopFunction &stopFunction, const CombinationMethod &combinationMethod, unsigned numberOfThreads) {

	size_t	referencePopulationSize = elitePopulationSize+diversePopulationSize,
			totalPopulationSize = referencePopulationSize + referencePopulationSize/2,
			candidatesPerThread; 

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

	thread *threads = new thread[numberOfThreads];

	if (referencePopulationSize&1) {
		throw invalid_argument("elitePopulationSize+diversePopulationSize must be an even number");
	} 
	
	if (candidatePopulation.size() < numberOfThreads) {
		throw invalid_argument("diversePopulationSize must be at least double the number of threads"); 
	}

	metrics.executionBegin = chrono::high_resolution_clock::now();

	parallel_for (referencePopulation.begin(), referencePopulation.end(), numberOfThreads) {
		Solution constructedSolution = localSearchHeuristic(graph, constructHeuristicSolution(graph), localSearchStopFunction);
		*i = {constructedSolution, graph.totalPenalty(constructedSolution)};
	} end_parallel_for;

	candidatesPerThread = candidatePopulation.size()/numberOfThreads;
	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	while (stopFunction(metrics)) {

		parallel_for (0, candidatePopulation.size(), numberOfThreads) {

			individual1 = &referencePopulation[i*2];
			individual2 = &referencePopulation[i*2+1];

			candidatePopulation[i].solution = combinationMethod(graph, &individual1->solution, &individual2->solution);
			candidatePopulation[i].solution = localSearchHeuristic(graph, candidatePopulation[i].solution, localSearchStopFunction);
			candidatePopulation[i].penalty = graph.totalPenalty(candidatePopulation[i].solution);

		} end_parallel_for;

		sort(population.begin(), population.end());

		diversify(graph, elitePopulation, diversePopulation, candidatePopulation, numberOfThreads);
		shuffle(referencePopulation.begin(), referencePopulation.end(), randomEngine);

		metrics.numberOfIterations++;

	} 
	
	delete [] threads;

	return population[0].solution;
}
