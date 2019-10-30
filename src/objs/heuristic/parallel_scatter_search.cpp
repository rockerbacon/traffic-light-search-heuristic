#include "heuristic/heuristic.h"
#include "heuristic/population.h"
#include "parallel/parallel.h"
#include <vector>
#include <random>
#include <algorithm>
#include <thread>
#include <mutex>

using namespace traffic;
using namespace std;
using namespace heuristic;

mutex coutMutex;
ostream & operator<< (ostream &stream, const Solution &solution) {
	for (Vertex v = 0; v < solution.getNumberOfVertices(); v++) {
		stream << solution.getTiming(v) << ", ";
	}
	return stream;
}

void recalculateDistances(const Graph &graph, Individual* individual, const vector<Individual*>::iterator &begin, const vector<Individual*>::iterator &end, unsigned availableThreads) {
	#pragma omp parallel for num_threads(availableThreads)
	for (auto i = begin; i < end; i++) {
		auto currentDistance = distance(graph, individual->solution, (*i)->solution);
		if (currentDistance < (*i)->minimumDistance) {
			(*i)->minimumDistance = currentDistance;
		}
	}
}

Population<Individual*> combineAndDiversify (
	const Graph &graph,
	PopulationInterface<Individual*> &leftPopulation,
   	PopulationInterface<Individual*> &rightPopulation,
	size_t elitePopulationSize,
	size_t diversePopulationSize,
	unsigned availableThreads
) {

	Population<Individual*> combinedPopulation(scatterSearchPopulationSize(elitePopulationSize, diversePopulationSize));

	size_t combinedElements;
	auto leftPopulationBegin = leftPopulation.begin();
	auto leftPopulationEnd = leftPopulation.end();
	auto rightPopulationBegin = rightPopulation.begin();
	auto rightPopulationEnd = rightPopulation.end();

	auto referencePopulationSize = elitePopulationSize + diversePopulationSize;

	vector<Individual*>::iterator leftChosenIndividual;
	vector<Individual*>::iterator rightChosenIndividual;

	// insert elite elements
	for (combinedElements = 0; combinedElements < elitePopulationSize; combinedElements++) {
		if ((*leftPopulationBegin)->penalty < (*rightPopulationBegin)->penalty) {
			combinedPopulation[combinedElements] = *leftPopulationBegin;
			recalculateDistances(graph, combinedPopulation[combinedElements], rightPopulationBegin, rightPopulationEnd, availableThreads);
			leftPopulationBegin++;
		} else {
			combinedPopulation[combinedElements] = *rightPopulationBegin;
			recalculateDistances(graph, combinedPopulation[combinedElements], leftPopulationBegin, leftPopulationEnd, availableThreads);
			rightPopulationBegin++;
		}
	}

	// pick most diverse elements
	for ( ; combinedElements < referencePopulationSize; combinedElements++) {

		#pragma omp sections
		{
			#pragma omp section
			leftChosenIndividual = max_element(leftPopulationBegin, leftPopulationEnd, [](const auto &a, const auto &b) { return a->minimumDistance > b->minimumDistance; });
			#pragma omp section
			rightChosenIndividual = max_element(rightPopulationBegin, rightPopulationEnd, [](const auto &a, const auto &b) { return a->minimumDistance > b->minimumDistance; });
		}
		if (leftPopulationBegin != leftPopulationEnd && (rightPopulationBegin == rightPopulationEnd || (*leftChosenIndividual)->minimumDistance > (*rightChosenIndividual)->minimumDistance)) {

			swap(*leftPopulationBegin, *leftChosenIndividual);
			combinedPopulation[combinedElements] = *leftPopulationBegin;
			leftPopulationBegin++;

			recalculateDistances(graph, combinedPopulation[combinedElements], rightPopulationBegin, rightPopulationEnd, availableThreads);

		} else {

			swap(*rightPopulationBegin, *rightChosenIndividual);
			combinedPopulation[combinedElements] = *rightPopulationBegin;
			rightPopulationBegin++;

			recalculateDistances(graph, combinedPopulation[combinedElements], leftPopulationBegin, leftPopulationEnd, availableThreads);

		}

	}

	// fill candidate population with the remains
	while (leftPopulationBegin != leftPopulationEnd) {
		combinedPopulation[combinedElements++] = *leftPopulationBegin;
		leftPopulationBegin++;
	}

	while (rightPopulationBegin != rightPopulationEnd) {
		combinedPopulation[combinedElements++] = *rightPopulationBegin;
		rightPopulationBegin++;
	}

	return combinedPopulation;

}

Population<Individual*> bottomUpTreeDiversify(const Graph &graph, vector<ScatterSearchPopulation<Individual*>> &population, vector<mutex> &mutex, size_t populationBegin, size_t populationEnd, size_t elitePopulationSize, size_t diversePopulationSize) {

	if (populationEnd-populationBegin < 2) {

		Population<Individual*> populationCopy(population[populationBegin].total.size());
		mutex[populationBegin].lock();
			for (size_t i = 0; i < populationCopy.size(); i++) {
				populationCopy[i] = population[populationBegin].total[i];
			}
		mutex[populationBegin].unlock();

		return populationCopy;
	} else {

		Population<Individual*> leftHalf, rightHalf;
		auto thread1 = thread([&]() {
			leftHalf = bottomUpTreeDiversify(graph, population, mutex, populationBegin, (populationBegin+populationEnd)/2, elitePopulationSize/2, diversePopulationSize/2);
		});
		auto thread2 = thread([&]() {
			rightHalf = bottomUpTreeDiversify(graph, population, mutex, (populationBegin+populationEnd)/2, populationEnd, elitePopulationSize/2, diversePopulationSize/2);
		});
		thread1.join();
		thread2.join();

		return combineAndDiversify(graph, leftHalf, rightHalf, elitePopulationSize, diversePopulationSize, population.size());
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

	StopFunction diverseLocalSearchStopFunction = stop_function_factory::numberOfIterations(localSearchIterations);
	StopFunction eliteLocalSearchStopFunction = stop_function_factory::numberOfIterations(localSearchIterations*10);

	Population<Individual> totalPopulation(scatterSearchPopulationSize(elitePopulationSize, diversePopulationSize), graph.getNumberOfVertices());
	Population<Individual*> totalPopulationReferences(totalPopulation.size());
	ScatterSearchPopulation<Individual*> subdividedTotalPopulation(totalPopulationReferences, elitePopulationSize, diversePopulationSize);

	Population<Individual*> arrangedPopulation(totalPopulation.size());

	vector<ScatterSearchPopulation<Individual*>> population(numberOfThreads);

	const auto threadPopulationSize = totalPopulation.size()/numberOfThreads;
	const auto threadElitePopulationSize = elitePopulationSize/numberOfThreads;
	const auto threadDiversePopulationSize = diversePopulationSize/numberOfThreads;
	const auto threadCandidatePopulationSize = (threadElitePopulationSize+threadDiversePopulationSize)/2;

	metrics.executionBegin = chrono::high_resolution_clock::now();

	for_each_thread (numberOfThreads) {

		auto elitePopulationBegin = threadElitePopulationSize*thread_i;
		auto elitePopulationEnd = threadElitePopulationSize*(thread_i+1);
		auto diversePopulationBegin = elitePopulationSize + threadDiversePopulationSize*thread_i;
		auto diversePopulationEnd = elitePopulationSize + threadDiversePopulationSize*(thread_i+1);
		auto candidatePopulationBegin = elitePopulationSize+diversePopulationSize + threadCandidatePopulationSize*thread_i;
		auto candidatePopulationEnd = elitePopulationSize+diversePopulationSize + threadCandidatePopulationSize*(thread_i+1);

		auto threadPopulation = arrangedPopulation.slice(threadPopulationSize*thread_i, threadPopulationSize*(thread_i+1));
		population[thread_i] = ScatterSearchPopulation<Individual*>(threadPopulation, threadElitePopulationSize, threadDiversePopulationSize);

		for (auto i = elitePopulationBegin; i < elitePopulationEnd; i++) {
			totalPopulation[i].solution = constructHeuristicSolution(graph);
			totalPopulation[i].solution = localSearchHeuristic(graph, totalPopulation[i].solution, eliteLocalSearchStopFunction);
			totalPopulation[i].penalty = graph.totalPenalty(totalPopulation[i].solution);
			subdividedTotalPopulation.total[i] = &totalPopulation[i];
		}

		for (auto i = diversePopulationBegin; i < diversePopulationEnd; i++) {
			totalPopulation[i].solution = constructHeuristicSolution(graph);
			totalPopulation[i].penalty = graph.totalPenalty(totalPopulation[i].solution);
			subdividedTotalPopulation.total[i] = &totalPopulation[i];
		}

		for (auto i = candidatePopulationBegin; i < candidatePopulationEnd; i++) {
			subdividedTotalPopulation.total[i] = &totalPopulation[i];
		}

	} end_for_each_thread;

	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	while (stopFunction(metrics)) {

		for (unsigned thread_i = 0; thread_i < numberOfThreads; thread_i++) {
			arrangePopulation(subdividedTotalPopulation.elite, population[thread_i].elite, thread_i);
			arrangePopulation(subdividedTotalPopulation.diverse, population[thread_i].diverse, thread_i);
			arrangePopulation(subdividedTotalPopulation.candidate, population[thread_i].candidate, thread_i);
		}

		vector<mutex> populationMutex(numberOfThreads);
		for_each_thread (numberOfThreads) {

			if (thread_i != 0) populationMutex[thread_i].lock();
				random_device seeder;
				mt19937 randomEngine(seeder());

				Individual *individual1, *individual2;

				shuffle(population[thread_i].reference.begin(), population[thread_i].reference.end(), randomEngine);

				for (size_t i = 0; i < population[thread_i].candidate.size(); i++) {

					individual1 = population[thread_i].reference[i*2];
					individual2 = population[thread_i].reference[i*2+1];

					population[thread_i].candidate[i]->solution = combinationMethod(graph, &individual1->solution, &individual2->solution);
					population[thread_i].candidate[i]->solution = localSearchHeuristic(graph, population[thread_i].candidate[i]->solution, diverseLocalSearchStopFunction);
					population[thread_i].candidate[i]->penalty = graph.totalPenalty(population[thread_i].candidate[i]->solution);

				}

				sort(population[thread_i].total.begin(), population[thread_i].total.end(), [](auto a, auto b) { return a->penalty < b->penalty; });

				diversify(graph, population[thread_i]);

				if (thread_i == 0) {
					auto nextPopulation = bottomUpTreeDiversify(graph, population, populationMutex, 0, numberOfThreads, elitePopulationSize, diversePopulationSize);
					for (size_t i = 0; i < nextPopulation.size(); i++) {
						subdividedTotalPopulation.total[i] = nextPopulation[i];
					}
				}
			if (thread_i != 0) populationMutex[thread_i].unlock();

		} end_for_each_thread;

		metrics.numberOfIterations++;

	}

	return (*min_element(subdividedTotalPopulation.elite.begin(), subdividedTotalPopulation.elite.end(), [](auto a, auto b) { return a->penalty < b->penalty; }))->solution;

}
