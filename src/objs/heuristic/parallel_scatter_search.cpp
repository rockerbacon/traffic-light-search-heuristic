#include "heuristic.h"
#include "population.h"
#include "../parallel/macros.h"
#include <vector>
#include <random>
#include <algorithm>
#include <thread>
#include <mutex>
#include <sstream>
#include "../parallel/reusable_thread.h"

#include <iostream>

//#define DELAYED_COMBINATION

using namespace traffic;
using namespace std;
using namespace heuristic;
using namespace ::parallel;

namespace global {
	thread_pile *threads;
	thread_pile::slice_t *threads_depth1;
}

void recalculateDistances(
		const Graph &graph,
		const Individual& individual,
		const vector<Individual>::iterator &begin,
		const vector<Individual>::iterator &end,
		thread_pile::slice_t &availableThreads
) {
	using_threads(availableThreads);
	parallel_for (begin, end) {
		auto currentDistance = distance(graph, individual.solution, i->solution);
		if (currentDistance < i->minimumDistance) {
			i->minimumDistance = currentDistance;
		}
	} end_parallel_for;
}
/*
tuple<TimeUnit, Population<Individual*>> simulateExchange(
	const Graph& graph,
	const PopulationInterface<Individual*>::iterator& populationIndividual,
	const PopulationInterface<Individual*>::iterator& candidateIndividual,
	PopulationInterface<Individual*>& population,
	thread_pile::slice_t& availableThreads
) {
	Population<Individual*> simulatedPopulation(population.begin(), population.end());

	auto simulationExchangeCandidate = simulatedPopulation.begin()+(populationIndividual - population.begin());
	swap(*simulationExchangeCandidate, *candidateIndividual);

	swap(*simulationExchangeCandidate, *simulatedPopulation.begin());
	auto minimumDistance = recalculateDistances(graph, *candidateIndividual, simulatedPopulation.begin()+1, simulatedPopulation.end(), availableThreads);
	swap(*simulationExchangeCandidate, *simulatedPopulation.begin());
	return make_tuple(minimumDistance, simulatedPopulation);
}
*/

bool lowestPenalty(const Individual& a, const Individual& b) {
	return a.penalty < b.penalty;
}
bool greatestMinimumDistance(const Individual& a, const Individual& b) {
	return a.minimumDistance > b.minimumDistance;
}

void exchangeDiscardedIndividuals (
	const Graph &graph,
	vector<ScatterSearchPopulation<Individual>> &populations,
	size_t populationOffsetBegin,
	size_t populationOffsetEnd,
	Population<Individual> &discardedPopulation,
	thread_pile::slice_t &availableThreads
) {
	//cerr << ("exchanging from " + to_string(populationOffsetBegin) + " to " + to_string(populationOffsetEnd)) << endl;
	auto populationEnd = populations.begin()+populationOffsetEnd;
	for (auto population_it = populations.begin()+populationOffsetBegin; population_it < populationEnd; population_it++) {
		auto& population = *population_it;

		using_threads(availableThreads);
		parallel_for (discardedPopulation.begin(), discardedPopulation.end()) {
			i->minimumDistance = numeric_limits<TimeUnit>::max();
		} end_parallel_for;

		for (auto& referenceIndividual : population.reference) {
			recalculateDistances(graph, referenceIndividual, discardedPopulation.begin(), discardedPopulation.end(), availableThreads);
		}

		auto discardedPopulationBegin = discardedPopulation.begin();

		// exchange elite individuals
		auto elitePopulationBegin = population.elite.begin();
		auto bestDiscardedIndividual = min_element(discardedPopulationBegin, discardedPopulation.end(), lowestPenalty);
		while (elitePopulationBegin->penalty > bestDiscardedIndividual->penalty && elitePopulationBegin < population.elite.end()) {
			iter_swap(discardedPopulationBegin, bestDiscardedIndividual);
			iter_swap(elitePopulationBegin, discardedPopulationBegin);
			discardedPopulationBegin++;
			recalculateDistances(graph, *elitePopulationBegin, population.diverse.begin(), population.diverse.end(), availableThreads);
			recalculateDistances(graph, *elitePopulationBegin, discardedPopulationBegin, discardedPopulation.end(), availableThreads);
			elitePopulationBegin++;
			bestDiscardedIndividual = max_element(discardedPopulationBegin, discardedPopulation.end(), lowestPenalty);
		}
		//cerr << ("updated elite from " + to_string(population_it - populations.begin())) << endl;

		// exchange diverse individuals
		auto diversePopulationBegin = population.diverse.begin();
		auto bestDiverseIndividual = max_element(diversePopulationBegin, population.diverse.end(), greatestMinimumDistance);
		bestDiscardedIndividual = max_element(discardedPopulationBegin, discardedPopulation.end(), greatestMinimumDistance);
		while (
			discardedPopulationBegin < discardedPopulation.end()
		&&	diversePopulationBegin < population.diverse.end()
		&&	bestDiverseIndividual->minimumDistance < bestDiscardedIndividual->minimumDistance
		) {
			iter_swap(discardedPopulationBegin, bestDiscardedIndividual);
			iter_swap(discardedPopulationBegin, bestDiscardedIndividual);
			iter_swap(diversePopulationBegin, bestDiverseIndividual);
			iter_swap(diversePopulationBegin, discardedPopulationBegin);
			discardedPopulationBegin++;
			if (population.diverse.end() - diversePopulationBegin > 1) {
				recalculateDistances(graph, *diversePopulationBegin, diversePopulationBegin+1, population.diverse.end(), availableThreads);
				recalculateDistances(graph, *diversePopulationBegin, discardedPopulationBegin, discardedPopulation.end(), availableThreads);
			}
			diversePopulationBegin++;
			bestDiscardedIndividual = max_element(discardedPopulationBegin, discardedPopulation.end(), greatestMinimumDistance);
			bestDiverseIndividual = max_element(diversePopulationBegin, population.diverse.end(), greatestMinimumDistance);
		}
		//cerr << ("updated diverse from " + to_string(population_it - populations.begin())) << endl;
	}
	//cerr << ("finished from " + to_string(populationOffsetBegin) + " to " + to_string(populationOffsetEnd)) << endl;
}

Population<Individual> bottomUpTreeDiversify(const Graph &graph, vector<ScatterSearchPopulation<Individual>> &population, size_t populationBegin, size_t populationEnd, size_t elitePopulationSize, size_t diversePopulationSize) {

	if (populationEnd-populationBegin < 2) {
		Population<Individual> baseDiscardedPopulation(population[populationBegin].candidate.begin(), population[populationBegin].candidate.end());
		return baseDiscardedPopulation;
	} else {

		Population<Individual> leftDiscardedPopulation,
		   						rightDiscardedPopulation;
		auto rightPopulationBegin = (populationBegin+populationEnd)/2;
		auto& neighborThread = (*global::threads)[rightPopulationBegin];

		auto rightHalfFuture = neighborThread.exec([&]() {
			rightDiscardedPopulation = bottomUpTreeDiversify(graph, population, rightPopulationBegin, populationEnd, elitePopulationSize/2, diversePopulationSize/2);
		});

		leftDiscardedPopulation = bottomUpTreeDiversify(graph, population, populationBegin, rightPopulationBegin, elitePopulationSize/2, diversePopulationSize/2);

		rightHalfFuture.wait();

		auto availableThreads = global::threads_depth1->slice(populationBegin, populationEnd);
		exchangeDiscardedIndividuals(graph, population, populationBegin, rightPopulationBegin, rightDiscardedPopulation, availableThreads);
		exchangeDiscardedIndividuals(graph, population, rightPopulationBegin, populationEnd, leftDiscardedPopulation, availableThreads);

		Population<Individual> totalDiscardedPopulation;
		totalDiscardedPopulation.reserve(rightDiscardedPopulation.size()+leftDiscardedPopulation.size());
		totalDiscardedPopulation.insert(totalDiscardedPopulation.end(), leftDiscardedPopulation.begin(), leftDiscardedPopulation.end());
		totalDiscardedPopulation.insert(totalDiscardedPopulation.end(), rightDiscardedPopulation.begin(), rightDiscardedPopulation.end());

		return totalDiscardedPopulation;
	}
}

unsigned brianKernighanCountBitsSet (unsigned number) {
	unsigned count = 0;
	while (number > 0) {
		number &= number-1;
		count++;
	}
	return count;
}

void arrangePopulation (PopulationInterface<Individual*> &totalPopulation, PopulationInterface<Individual*> &population, unsigned thread_i) {

	auto totalPopulationBegin = population.size()*thread_i;
	auto totalPopulationEnd = totalPopulationBegin+population.size();
	size_t total_i, population_i;

	for (total_i = totalPopulationBegin, population_i = 0; total_i < totalPopulationEnd; total_i++, population_i++) {
		population[population_i]  = totalPopulation[total_i];
	}

}

Solution heuristic::parallel::scatterSearch (const Graph &graph, size_t elitePopulationSize, size_t diversePopulationSize, size_t localSearchIterations, const StopFunction &stopFunction, const CombinationMethod &combinationMethod, unsigned numberOfThreads) {
	if (elitePopulationSize%numberOfThreads != 0) {
		throw invalid_argument("elitePopulationSize must be a multiple of the number of threads");
	}
	if ((elitePopulationSize+diversePopulationSize) % numberOfThreads != 0) {
		throw invalid_argument("elitePopulationSize+diversePopulationSize must be a multiple of the number of threads");
	}
	if (((elitePopulationSize+diversePopulationSize)/numberOfThreads)&1) {
		throw invalid_argument("(elitePopulationSize+diversePopulationSize)/numberOfThreads must be an even number");
	}
	if (brianKernighanCountBitsSet(numberOfThreads) != 1) {
		throw invalid_argument("numberOfThreads must be a power of 2");
	}

	Metrics metrics;
#ifdef DELAYED_COMBINATION
	atomic<bool> combinationSignal;
#endif

	thread_pile threads(numberOfThreads, 2);
	thread_pile::slice_t threads_depth1 = threads.depth(1);
	global::threads = &threads;
	global::threads_depth1 = &threads_depth1;

	StopFunction diverseLocalSearchStopFunction = stop_function_factory::numberOfIterations(localSearchIterations);
	StopFunction eliteLocalSearchStopFunction = stop_function_factory::numberOfIterations(localSearchIterations*10);

	Population<Individual> totalPopulation(scatterSearchPopulationSize(elitePopulationSize, diversePopulationSize), graph.getNumberOfVertices());
	vector<ScatterSearchPopulation<Individual>> populations(numberOfThreads);

#ifdef DELAYED_COMBINATION
	vector<TimeUnit> minimumPenalty(numberOfThreads, numeric_limits<TimeUnit>::max());
	vector<TimeUnit> minimumDistance(numberOfThreads, numeric_limits<TimeUnit>::max());
#endif

	const auto threadPopulationSize = totalPopulation.size()/numberOfThreads;
	const auto threadElitePopulationSize = elitePopulationSize/numberOfThreads;
	const auto threadDiversePopulationSize = diversePopulationSize/numberOfThreads;

	metrics.executionBegin = chrono::high_resolution_clock::now();

	using_threads(*global::threads);
	for_each_thread {
		auto threadPopulation = totalPopulation.slice(threadPopulationSize*thread_i, threadPopulationSize*(thread_i+1));
		populations[thread_i] = ScatterSearchPopulation<Individual>(threadPopulation, threadElitePopulationSize, threadDiversePopulationSize);

		for (auto& eliteIndividual : populations[thread_i].elite) {
			auto initialSolution = localSearchHeuristic(graph, constructHeuristicSolution(graph), eliteLocalSearchStopFunction);
			eliteIndividual = {
				initialSolution,
				graph.totalPenalty(initialSolution),
				numeric_limits<TimeUnit>::max()
			};
		}

		for (auto& diverseIndividual : populations[thread_i].diverse) {
			auto initialSolution = localSearchHeuristic(graph, constructHeuristicSolution(graph), diverseLocalSearchStopFunction);
			diverseIndividual = {
				initialSolution,
				graph.totalPenalty(initialSolution),
				numeric_limits<TimeUnit>::max()
			};
		}
	} end_for_each_thread;

	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	metrics.penalty = numeric_limits<TimeUnit>::max();
	while (stopFunction(metrics)) {

		for_each_thread {
			auto& population = populations[thread_i];

			random_device seeder;
			mt19937 randomEngine(seeder());

			shuffle(populations[thread_i].reference.begin(), populations[thread_i].reference.end(), randomEngine);

			for (size_t i = 0; i < populations[thread_i].candidate.size(); i++) {

				auto& individual1 = populations[thread_i].reference[i*2];
				auto& individual2 = populations[thread_i].reference[i*2+1];

				population.candidate[i].solution = combinationMethod(graph, individual1.solution, individual2.solution);
				population.candidate[i].solution = localSearchHeuristic(graph, population.candidate[i].solution, diverseLocalSearchStopFunction);
				population.candidate[i].penalty = graph.totalPenalty(populations[thread_i].candidate[i].solution);

			}

			sort(population.total.begin(), population.total.end(), lowestPenalty);

		#ifdef DELAYED_COMBINATION
			if(population.elite[0].penalty < minimumPenalty[thread_i]) {
				combinationSignal.store(true);
				minimumPenalty[thread_i] = population.elite[0].penalty;
			}
		#endif

		#ifdef DELAYED_COMBINATION
			auto iterationMinimumDistance =
		#endif
				diversify(graph, populations[thread_i]);

		#ifdef DELAYED_COMBINATION
			if (iterationMinimumDistance < minimumDistance[thread_i]) {
				combinationSignal.store(true);
				minimumDistance[thread_i] = iterationMinimumDistance;
			}
		#endif

			if	(
				thread_i == 0
			#ifdef DELAYED_COMBINATION
			   	&& combinationSignal.load()
			#endif
				) {
				//bottomUpTreeDiversify(graph, population, 0, numberOfThreads, elitePopulationSize, diversePopulationSize);
			#ifdef DELAYED_COMBINATION
				combinationSignal.store(false);
			#endif
			}

		} end_for_each_thread;

		metrics.numberOfIterations++;

	}

	auto bestIndividual = populations[0].elite.begin();
	for (auto& population : populations) {
		for (auto eliteIndividual = population.elite.begin(); eliteIndividual < population.elite.end(); eliteIndividual++) {
			if (eliteIndividual->penalty < bestIndividual->penalty) {
				bestIndividual = eliteIndividual;
			}
		}
	}
	return bestIndividual->solution;

}
