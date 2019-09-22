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

// TODO clean after debugging
mutex coutMutex;
ostream& operator<<(ostream &stream, const Individual &individual) {
	for (Vertex v = 0; v < individual.solution.getNumberOfVertices(); v++) {
		stream << individual.solution.getTiming(v) << ", ";
	}
	return stream;
}
		

vector<Individual>::iterator recalculateDistancesToElitePopulation(const Graph &graph, PopulationSlice &population, PopulationInterface &elitePopulation) {

	TimeUnit currentDistance;
	TimeUnit greatestMinimumDistance = numeric_limits<TimeUnit>::min();
	vector<Individual>::iterator chosenIndividual;

	for (auto i = population.begin(); i != population.end(); i++) {

		for (auto &j : elitePopulation) {
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

	return chosenIndividual;

}

vector<Individual>::iterator recalculateDistances(const Graph &graph, const vector<Individual>::iterator &individual, PopulationInterface &population) {
	TimeUnit currentDistance;
	TimeUnit greatestMinimumDistance = numeric_limits<TimeUnit>::min();
	vector<Individual>::iterator chosenIndividual;

	for (auto i = population.begin(); i != population.end(); i++) {
		currentDistance = distance(graph, individual->solution, i->solution);
		if (currentDistance < i->minimumDistance) {
			i->minimumDistance = currentDistance;
		}
		if (i->minimumDistance > greatestMinimumDistance) {
			greatestMinimumDistance = i->minimumDistance;
			chosenIndividual = i;
		}
	}

	return chosenIndividual;
}

void combineAndDiversify (
	const Graph &graph,
	ScatterSearchPopulation &leftPopulation,
   	ScatterSearchPopulation &rightPopulation,
	PopulationInterface &nextPopulation
) {

	vector<Individual>::iterator *chosenIndividual;
	size_t individualsChosen = 0;

	// insert elite elements
	for (auto &i : leftPopulation.elite) {
		swap(nextPopulation[individualsChosen++], i);
	}
	for (auto &i : rightPopulation.elite) {
		swap(nextPopulation[individualsChosen++], i);
	}

	auto leftChosenIndividual = recalculateDistancesToElitePopulation(graph, leftPopulation.diverse, rightPopulation.elite);
	auto rightChosenIndividual = recalculateDistancesToElitePopulation(graph, rightPopulation.diverse, leftPopulation.elite);

	if (leftChosenIndividual->minimumDistance > rightChosenIndividual->minimumDistance) {
		chosenIndividual = &leftChosenIndividual;
	} else {
		chosenIndividual = &rightChosenIndividual;
	}

	swap(nextPopulation[individualsChosen++], **chosenIndividual);

	while (individualsChosen < nextPopulation.size()) {
	
		if (chosenIndividual == &leftChosenIndividual) {
			leftChosenIndividual = max_element(leftPopulation.diverse.begin(), leftPopulation.diverse.end(), [](const auto &a, const auto &b) { return a.minimumDistance > b.minimumDistance; });
			rightChosenIndividual = recalculateDistances(graph, *chosenIndividual, rightPopulation.diverse);
		} else {
			leftChosenIndividual = recalculateDistances(graph, *chosenIndividual, leftPopulation.diverse);
			rightChosenIndividual = max_element(rightPopulation.diverse.begin(), rightPopulation.diverse.end(), [](const auto &a, const auto &b) { return a.minimumDistance > b.minimumDistance; });
		}	

		if (leftChosenIndividual->minimumDistance > rightChosenIndividual->minimumDistance) {
			chosenIndividual = &leftChosenIndividual;
		} else {
			chosenIndividual = &rightChosenIndividual;
		}

		swap(nextPopulation[individualsChosen++], **chosenIndividual);

	}

}

void bottomUpTreeDiversify(const Graph &graph, PopulationInterface &combinedPopulation, ScatterSearchPopulation &currentPopulationLeftHalf, ScatterSearchPopulation &currentPopulationRightHalf, vector<mutex> &mutex, size_t currentPopulation_i, size_t combinationsCount, unsigned numberOfThreads) {

	auto populationToWait_i = currentPopulation_i+combinationsCount;

	if (populationToWait_i < numberOfThreads) {

		mutex[populationToWait_i].lock();
			combineAndDiversify(graph, currentPopulationLeftHalf, currentPopulationRightHalf, combinedPopulation);

			auto expandedPopulation = PopulationSlice(combinedPopulation.begin(), combinedPopulation.begin()+2*combinedPopulation.size());

			auto nextPopulationLeftHalf = ScatterSearchPopulation(combinedPopulation, currentPopulationLeftHalf.elite.size()+currentPopulationRightHalf.elite.size(), currentPopulationLeftHalf.diverse.size()+currentPopulationRightHalf.diverse.size());

			auto nextPopulationRightHalfSlice = PopulationSlice(currentPopulationRightHalf.total.end(), currentPopulationRightHalf.total.end()+combinedPopulation.size());
			auto nextPopulationRightHalf = ScatterSearchPopulation(nextPopulationRightHalfSlice, nextPopulationLeftHalf.elite.size(), nextPopulationLeftHalf.diverse.size());

			bottomUpTreeDiversify(graph, expandedPopulation, nextPopulationLeftHalf, nextPopulationRightHalf, mutex, currentPopulation_i, combinationsCount*2, numberOfThreads);
		mutex[populationToWait_i].unlock();

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

void arrangePopulation (ScatterSearchPopulation &totalPopulation, ScatterSearchPopulation &population, unsigned thread_i) {

	auto elitePopulationBegin = totalPopulation.elite.begin()+population.elite.size()*thread_i;
	auto elitePopulationEnd = totalPopulation.elite.begin()+population.elite.size()*(thread_i+1);
	auto candidatePopulationBegin = totalPopulation.candidate.begin()+population.candidate.size()*thread_i;
	auto candidatePopulationEnd = totalPopulation.candidate.begin()+population.candidate.size()*(thread_i+1);
	vector<Individual>::iterator unifiedDiverseIndividual;

	unifiedDiverseIndividual = population.elite.begin();
	for (auto unifiedEliteIndividual = elitePopulationBegin; unifiedEliteIndividual != elitePopulationEnd; unifiedEliteIndividual++) {
		swap(*unifiedEliteIndividual, *unifiedDiverseIndividual);
		unifiedDiverseIndividual++;
	}
	unifiedDiverseIndividual = population.candidate.begin();
	for (auto unifiedCandidateIndividual = candidatePopulationBegin; unifiedCandidateIndividual != candidatePopulationEnd; unifiedCandidateIndividual++) {
		swap(*unifiedCandidateIndividual, *unifiedDiverseIndividual);
		unifiedDiverseIndividual++;
	}

}

Solution heuristic::parallel::scatterSearch (const Graph &graph, size_t elitePopulationSize, size_t diversePopulationSize, size_t localSearchIterations, const StopFunction &stopFunction, const CombinationMethod &combinationMethod, unsigned numberOfThreads) {
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

	vector<Population> totalPopulation; totalPopulation.reserve(2);
	vector<ScatterSearchPopulation> totalSubdividedPopulation(2);
	size_t currentPopulation_i, nextPopulation_i;
	vector<vector<ScatterSearchPopulation>> population(2);

	for (size_t i = 0; i < 2; i++) {
		totalPopulation.push_back( Population(scatterSearchPopulationSize(elitePopulationSize, diversePopulationSize), graph.getNumberOfVertices()) );
		totalSubdividedPopulation[i] = ScatterSearchPopulation(totalPopulation.back(), elitePopulationSize, diversePopulationSize);
		population[i] = vector<ScatterSearchPopulation>(numberOfThreads);
	}

	auto threadPopulationSize = totalPopulation[0].size()/numberOfThreads;
	auto threadElitePopulationSize = elitePopulationSize/numberOfThreads;
	auto threadDiversePopulationSize = diversePopulationSize/numberOfThreads;
	auto threadReferencePopulationSize = threadElitePopulationSize+threadDiversePopulationSize;

	metrics.executionBegin = chrono::high_resolution_clock::now();

	for_each_thread (numberOfThreads) {

		for (size_t i = 0; i < 2; i++) {
			auto threadPopulation = totalPopulation[i].slice(threadPopulationSize*thread_i, threadPopulationSize*(thread_i+1));
			population[i][thread_i] = ScatterSearchPopulation(threadPopulation, threadElitePopulationSize, threadDiversePopulationSize);
		}
		
		for (auto i = threadElitePopulationSize*thread_i; i < threadElitePopulationSize*(thread_i+1); i++) {
			totalPopulation[0][i].solution = constructHeuristicSolution(graph);
			totalPopulation[0][i].solution = localSearchHeuristic(graph, totalPopulation[0][i].solution, eliteLocalSearchStopFunction);
			totalPopulation[0][i].penalty = graph.totalPenalty(totalPopulation[0][i].solution);
		}
		
		for (auto i = elitePopulationSize+threadDiversePopulationSize*thread_i; i < elitePopulationSize+threadDiversePopulationSize*(thread_i+1); i++) {
			totalPopulation[0][i].solution = constructHeuristicSolution(graph);
			totalPopulation[0][i].penalty = graph.totalPenalty(totalPopulation[0][i].solution);
		}

	} end_for_each_thread;

	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	nextPopulation_i = 0;
	while (stopFunction(metrics)) {

		vector<mutex> populationMutex(numberOfThreads);
		currentPopulation_i = nextPopulation_i;
		nextPopulation_i = (currentPopulation_i+1)%2;

				coutMutex.lock();
				cout << " before arrangement" << endl;
				for (auto &i : totalPopulation[currentPopulation_i]) {
					cout << i << " penalty " << i.penalty << endl;
				}
				coutMutex.unlock();

		for_each_thread (numberOfThreads) {
			arrangePopulation(totalSubdividedPopulation[currentPopulation_i], population[currentPopulation_i][thread_i], thread_i);
		} end_for_each_thread;

				coutMutex.lock();
				cout << " after arrangement" << endl;
				for (auto &i : totalPopulation[currentPopulation_i]) {
					cout << i << " penalty " << i.penalty << endl;
				}
				coutMutex.unlock();

		for_each_thread (numberOfThreads) {

			random_device seeder;
			mt19937 randomEngine(seeder());

			Individual *individual1, *individual2;

			populationMutex[thread_i].lock();

				shuffle(population[currentPopulation_i][thread_i].reference.begin(), population[currentPopulation_i][thread_i].reference.end(), randomEngine);

				for (size_t i = 0; i < population[currentPopulation_i][thread_i].candidate.size(); i++) {

					individual1 = &population[currentPopulation_i][thread_i].reference[i*2];
					individual2 = &population[currentPopulation_i][thread_i].reference[i*2+1];

					population[currentPopulation_i][thread_i].candidate[i].solution = combinationMethod(graph, &individual1->solution, &individual2->solution);
					population[currentPopulation_i][thread_i].candidate[i].solution = localSearchHeuristic(graph, population[currentPopulation_i][thread_i].candidate[i].solution, diverseLocalSearchStopFunction);
					population[currentPopulation_i][thread_i].candidate[i].penalty = graph.totalPenalty(population[currentPopulation_i][thread_i].candidate[i].solution);

				}

				sort(population[currentPopulation_i][thread_i].total.begin(), population[currentPopulation_i][thread_i].total.end());

				diversify(graph, population[currentPopulation_i][thread_i]);

				if ((thread_i&1) == 0) {
					auto nextPopulationSlice = totalPopulation[nextPopulation_i].slice(threadReferencePopulationSize*thread_i, threadReferencePopulationSize*(thread_i + 2));
					bottomUpTreeDiversify(graph, nextPopulationSlice, population[currentPopulation_i][thread_i], population[currentPopulation_i][thread_i+1], populationMutex, thread_i, 1, numberOfThreads);
				}

			populationMutex[thread_i].unlock();

		} end_for_each_thread;

		metrics.numberOfIterations++;

	}

	return min_element(totalPopulation[currentPopulation_i].begin(), totalPopulation[currentPopulation_i].begin()+elitePopulationSize)->solution;

}
