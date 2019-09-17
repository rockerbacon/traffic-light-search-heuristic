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

void recalculateDistancesToElitePopulation(const Graph &graph, PopulationSlice &population, PopulationSlice &elitePopulation, vector<Individual>::iterator &chosenIndividual, TimeUnit &greatestMinimumDistance) {

	TimeUnit currentDistance;

	for (auto i = population.begin(); i != population.end(); i++) {

		for (auto j : elitePopulation) {
			currentDistance = distance(graph, i->solution, j.solution);
			if (currentDistance < i->minimumDistance) {
				i->minimumDistance = currentDistance;
			}
		}

		if (i->minimumDistance > greatestMinimumDistance) {
			chosenIndividual = i;
			greatestMinimumDistance = i->minimumDistance;
		}

	}

}

void combinePopulations (
	const Graph &graph,
	PopulationSlice &leftElitePopulation, PopulationSlice &leftBattlingPopulation,
   	PopulationSlice &rightElitePopulation, PopulationSlice &rightBattlingPopulation,
	vector<Individual>::iterator newPopulationEnd
) {
	TimeUnit minusInfinity = numeric_limits<TimeUnit>::max();
	TimeUnit greatestMinimumDistance;
	TimeUnit currentDistance;
	vector<Individual>::iterator chosenIndividual, nextChosenIndividual;

	vector<Individual>::iterator nextGenerationEnd;
	vector<Individual>::iterator	battlingPopulationBegin = leftBattlingPopulation.begin(),
	   								battlingPopulationEnd = rightBattlingPopulation.end();

	// unify elite populations
	nextGenerationEnd = leftElitePopulation.end();

	for (auto i = rightElitePopulation.begin(); i != rightElitePopulation.end(); i++) {
		swap(*nextGenerationEnd, *i);
		nextGenerationEnd++;	
	}

	rightElitePopulation = PopulationSlice(leftElitePopulation.end(), nextGenerationEnd);
	// unify elite populations

	// recalculate distances to elite populations
	greatestMinimumDistance = minusInfinity;
	recalculateDistancesToElitePopulation(graph, leftBattlingPopulation, rightElitePopulation, chosenIndividual, greatestMinimumDistance);
	recalculateDistancesToElitePopulation(graph, rightBattlingPopulation, leftElitePopulation, chosenIndividual, greatestMinimumDistance);

	swap(*nextGenerationEnd, *chosenIndividual);
	nextGenerationEnd++;
	battlingPopulationBegin++;
	// recalculate distances to elite populations

	while (nextGenerationEnd != newPopulationEnd) {

		greatestMinimumDistance = minusInfinity;
		for (auto j = battlingPopulationBegin; j != battlingPopulationEnd; j++) {
			currentDistance = distance(graph, chosenIndividual->solution, j->solution);
			if (currentDistance > greatestMinimumDistance) {
				greatestMinimumDistance = currentDistance;
				nextChosenIndividual = j;
			}
		}

		chosenIndividual = nextChosenIndividual;
		swap(*nextGenerationEnd, *chosenIndividual);
		nextGenerationEnd++;
		battlingPopulationBegin++;

	}
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

	StopFunction diverseLocalSearchStopFunction = stop_function_factory::numberOfIterations(localSearchIterations);
	StopFunction eliteLocalSearchStopFunction = stop_function_factory::numberOfIterations(localSearchIterations*10);

	auto totalPopulationSize = 3*(elitePopulationSize+diversePopulationSize)/2;
	Population totalPopulation(totalPopulationSize, graph.getNumberOfVertices());
	vector<PopulationSlice> population(numberOfThreads);
	vector<PopulationSlice> elitePopulation(numberOfThreads);
	vector<PopulationSlice> diversePopulation(numberOfThreads);
	vector<PopulationSlice> candidatePopulation(numberOfThreads);
	vector<PopulationSlice> referencePopulation(numberOfThreads);

	auto threadPopulationSize = totalPopulation.size()/numberOfThreads;
	auto threadElitePopulationSize = elitePopulationSize/numberOfThreads;
	auto threadDiversePopulationSize = diversePopulationSize/numberOfThreads;
	auto threadCandidatePopulationSize = (threadElitePopulationSize + threadDiversePopulationSize)/2;

	metrics.executionBegin = chrono::high_resolution_clock::now();

	for_each_thread (numberOfThreads) {

		population[thread_i] = PopulationSlice(totalPopulation, threadPopulationSize*thread_i, threadPopulationSize*(thread_i+1));
		elitePopulation[thread_i] = PopulationSlice(population[thread_i], 0, threadElitePopulationSize);
		diversePopulation[thread_i] = PopulationSlice(elitePopulation[thread_i].end(), elitePopulation[thread_i].end()+threadDiversePopulationSize);
		candidatePopulation[thread_i] = PopulationSlice(diversePopulation[thread_i].end(), diversePopulation[thread_i].end()+threadCandidatePopulationSize);
		referencePopulation[thread_i] = PopulationSlice(elitePopulation[thread_i].begin(), diversePopulation[thread_i].end());
		
		for (auto i : elitePopulation[thread_i]) {
			i.solution = constructHeuristicSolution(graph);
			i.solution = localSearchHeuristic(graph, i.solution, eliteLocalSearchStopFunction);
			i.penalty = graph.totalPenalty(i.solution);
		}
		
		for (auto i : diversePopulation[thread_i]) {
			i.solution = constructHeuristicSolution(graph);
			i.penalty = graph.totalPenalty(i.solution);
		}

	} end_for_each_thread;

	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	while (stopFunction(metrics)) {

		for_each_thread (numberOfThreads) {

			random_device seeder;
			mt19937 randomEngine(seeder());

			Individual *individual1, *individual2;

			shuffle(referencePopulation[thread_i].begin(), referencePopulation[thread_i].end(), randomEngine);

			for (size_t i = 0; i < candidatePopulation[thread_i].size(); i++) {

				individual1 = &referencePopulation[thread_i][i*2];
				individual2 = &referencePopulation[thread_i][i*2+1];

				candidatePopulation[thread_i][i].solution = combinationMethod(graph, &individual1->solution, &individual2->solution);
				candidatePopulation[thread_i][i].solution = localSearchHeuristic(graph, candidatePopulation[thread_i][i].solution, diverseLocalSearchStopFunction);
				candidatePopulation[thread_i][i].penalty = graph.totalPenalty(candidatePopulation[thread_i][i].solution);

			}

			sort(population[thread_i].begin(), population[thread_i].end());

			diversify(graph, elitePopulation[thread_i], diversePopulation[thread_i], candidatePopulation[thread_i]);
			if ((thread_i&1) == 0) {
				bottomUpTreeDiversify(graph, elitePopulation, diversePopulation, candidatePopulation, thread_i, thread_i, thread_i+1, thread_i+1, numberOfThreads);
			}

		} end_for_each_thread;

		metrics.numberOfIterations++;

	}

	return totalPopulation[0].solution;

}
