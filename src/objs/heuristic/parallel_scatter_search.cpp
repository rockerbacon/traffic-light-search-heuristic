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

void bottomUpTreeDiversify(const Graph &graph, PopulationSlice &threadReferencePopulation, PopulationSlice &threadCandidatePopulation) {

}

Solution heuristic::parallel::scatterSearch (const Graph &graph, size_t elitePopulationSize, size_t diversePopulationSize, size_t localSearchIterations, const StopFunction &stopFunction, const CombinationMethod &combinationMethod, unsigned numberOfThreads) {
	if (elitePopulationSize % numberOfThreads != 0) {
		throw invalid_argument("elitePopulationSize must be a multiple of the number of threads");
	}
	if (diversePopulationSize % numberOfThreads != 0) {
		throw invalid_argument("diversePopulationSize must be a multiple of the number of threads");
	}
	if (numberOfThreads % 2 != 0) {
		throw invalid_argument("numberOfThreads must be an even number");
	}

	Metrics metrics;

	auto referencePopulationSize = elitePopulationSize + diversePopulationSize;
	auto candidatePopulationSize = referencePopulationSize/2;

	StopFunction diverseLocalSearchStopFunction = stop_function_factory::numberOfIterations(localSearchIterations);
	StopFunction eliteLocalSearchStopFunction = stop_function_factory::numberOfIterations(localSearchIterations*10);
	Population totalPopulation(referencePopulationSize+candidatePopulationSize, graph.getNumberOfVertices());
	PopulationSlice referencePopulation(totalPopulation, 0, referencePopulationSize);
	PopulationSlice candidatePopulation(referencePopulation, referencePopulationSize, referencePopulationSize+candidatePopulationSize);

	auto referenceIndividualsPerThread = (elitePopulationSize+diversePopulationSize)/numberOfThreads;

	metrics.executionBegin = chrono::high_resolution_clock::now();

	for_each_thread (numberOfThreads) {
		PopulationSlice threadPopulation(totalPopulation, referenceIndividualsPerThread*thread_i, referenceIndividualsPerThread*(thread_i+1));
		
		for (decltype(elitePopulationSize) i = 0; i < elitePopulationSize; i++) {
			threadPopulation[i].solution = constructHeuristicSolution(graph);
			threadPopulation[i].solution = localSearchHeuristic(graph, threadPopulation[i].solution, eliteLocalSearchStopFunction);
			threadPopulation[i].penalty = graph.totalPenalty(threadPopulation[i].solution);
		}
		
		for (decltype(diversePopulationSize) i = elitePopulationSize; i < elitePopulationSize+diversePopulationSize; i++) {
			threadPopulation[i].solution = constructHeuristicSolution(graph);
			threadPopulation[i].penalty = graph.totalPenalty(threadPopulation[i].solution);
		}
	} end_for_each_thread;

	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	while (stopFunction(metrics)) {

		for_each_thread (numberOfThreads) {

			random_device seeder;
			mt19937 randomEngine(seeder());

			PopulationSlice threadReferencePopulation(referencePopulation, referenceIndividualsPerThread*thread_i, referenceIndividualsPerThread*(thread_i+1));
			PopulationSlice threadCandidatePopulation(candidatePopulation, referenceIndividualsPerThread/2*thread_i, referenceIndividualsPerThread/2*(thread_i+1));
			Individual *individual1, *individual2;

			shuffle(threadReferencePopulation.begin(), threadReferencePopulation.end(), randomEngine);

			for (size_t i = 0; i < threadCandidatePopulation.size(); i++) {

				individual1 = &threadReferencePopulation[i*2];
				individual2 = &threadReferencePopulation[i*2+1];

				threadCandidatePopulation[i].solution = combinationMethod(graph, &individual1->solution, &individual2->solution);
				threadCandidatePopulation[i].solution = localSearchHeuristic(graph, threadCandidatePopulation[i].solution, diverseLocalSearchStopFunction);
				threadCandidatePopulation[i].penalty = graph.totalPenalty(threadCandidatePopulation[i].solution);

			}

			sort(threadReferencePopulation.begin(), threadCandidatePopulation.end());

			bottomUpTreeDiversify(graph, threadReferencePopulation, threadCandidatePopulation);

			metrics.numberOfIterations++;

		} end_for_each_thread;

	}

	return totalPopulation[0].solution;

}
