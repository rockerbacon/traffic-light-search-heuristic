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

vector<Individual*>::iterator recalculateDistancesToElitePopulation(const Graph &graph, PopulationSlice<Individual*> &population, PopulationInterface<Individual*> &elitePopulation) {

	TimeUnit currentDistance;
	TimeUnit greatestMinimumDistance = numeric_limits<TimeUnit>::min();
	vector<Individual*>::iterator chosenIndividual;

	for (auto i = population.begin(); i != population.end(); i++) {

		for (auto &j : elitePopulation) {
			currentDistance = distance(graph, (*i)->solution, (*j).solution);
			if (currentDistance < (*i)->minimumDistance) {
				(*i)->minimumDistance = currentDistance;
			}
		}

		if ((*i)->minimumDistance > greatestMinimumDistance) {
			chosenIndividual = i;
			greatestMinimumDistance = (*i)->minimumDistance;
		}

	}

	return chosenIndividual;

}

vector<Individual*>::iterator recalculateDistances(const Graph &graph, const vector<Individual*>::iterator &individual, const vector<Individual*>::iterator &begin, const vector<Individual*>::iterator &end) {
	TimeUnit currentDistance;
	TimeUnit greatestMinimumDistance = numeric_limits<TimeUnit>::min();
	vector<Individual*>::iterator chosenIndividual;

	for (auto i = begin; i != end; i++) {
		currentDistance = distance(graph, (*individual)->solution, (*i)->solution);
		if (currentDistance < (*i)->minimumDistance) {
			(*i)->minimumDistance = currentDistance;
		}
		if ((*i)->minimumDistance > greatestMinimumDistance) {
			greatestMinimumDistance = (*i)->minimumDistance;
			chosenIndividual = i;
		}
	}

	return chosenIndividual;
}

void combineAndDiversify (
	const Graph &graph,
	ScatterSearchPopulation<Individual*> &leftPopulation,
   	ScatterSearchPopulation<Individual*> &rightPopulation
) {

	auto leftPopulationEnd = leftPopulation.battling.end(); 
	auto rightPopulationBegin = rightPopulation.battling.begin();
	auto rightPopulationEnd = rightPopulation.battling.end();

	auto newReferencePopulationEnd = leftPopulation.total.begin() + leftPopulation.reference.size()+rightPopulation.reference.size();
	auto newElitePopulationEnd = leftPopulation.elite.begin() + leftPopulation.elite.size()+rightPopulation.elite.size();

	decltype(leftPopulationEnd) leftPopulationBegin;

	// insert elite elements
	for (leftPopulationBegin = leftPopulation.elite.begin(); leftPopulationBegin != newElitePopulationEnd; leftPopulationBegin++) {
		if ((*rightPopulation.elite.begin())->penalty < (*leftPopulationBegin)->penalty) {
			swap(*leftPopulationBegin, *rightPopulation.elite.begin());
		}
	}

	auto leftChosenIndividual = recalculateDistancesToElitePopulation(graph, leftPopulation.battling, rightPopulation.elite);
	auto rightChosenIndividual = recalculateDistancesToElitePopulation(graph, rightPopulation.battling, leftPopulation.elite);

	while (leftPopulationBegin != newReferencePopulationEnd) {
	
		if (leftPopulationBegin != leftPopulationEnd && (rightPopulationBegin == rightPopulationEnd || (*leftChosenIndividual)->minimumDistance > (*rightChosenIndividual)->minimumDistance)) {

			swap(*leftPopulationBegin, *leftChosenIndividual);
			leftPopulationBegin++;

			rightChosenIndividual = recalculateDistances(graph, leftChosenIndividual, rightPopulationBegin, rightPopulationEnd);
			leftChosenIndividual = max_element(leftPopulationBegin, leftPopulationEnd, [](const auto &a, const auto &b) { return a->minimumDistance > b->minimumDistance; });

		} else {

			swap(*rightPopulationBegin, *rightChosenIndividual);
			swap(*leftPopulationEnd, *rightPopulationBegin);
			swap(*leftPopulationBegin, *leftPopulationEnd);
			rightPopulationBegin++;
			leftPopulationBegin++;
			leftPopulationEnd++;

			leftChosenIndividual = recalculateDistances(graph, rightChosenIndividual, leftPopulationBegin, leftPopulationEnd);
			rightChosenIndividual = max_element(rightPopulationBegin, rightPopulationEnd, [](const auto &a, const auto &b) { return a->minimumDistance > b->minimumDistance; });

		}

	}

}

void bottomUpTreeDiversify(const Graph &graph, ScatterSearchPopulation<Individual*> &currentPopulationLeftHalf, ScatterSearchPopulation<Individual*> &currentPopulationRightHalf, vector<mutex> &mutex, size_t currentPopulation_i, size_t combinationsCount, unsigned numberOfThreads) {

	auto populationToWait_i = currentPopulation_i+combinationsCount;

	if (populationToWait_i < numberOfThreads) {

		mutex[populationToWait_i].lock();
/*
			coutMutex.lock();
			cout << currentPopulation_i << " left" << endl;
			for (auto i : currentPopulationLeftHalf.total) {
				cout << i.solution << " penalty " << i.penalty << endl;
			}
			cout << currentPopulation_i << " right" << endl;
			for (auto i : currentPopulationRightHalf.total) {
				cout << i.solution << " penalty " << i.penalty << endl;
			}*/
			combineAndDiversify(graph, currentPopulationLeftHalf, currentPopulationRightHalf);

			auto combinedPopulation = PopulationSlice<Individual*>(currentPopulationLeftHalf.total.begin(), currentPopulationLeftHalf.total.begin()+currentPopulationLeftHalf.reference.size()+currentPopulationRightHalf.reference.size()); 
			/*cout << currentPopulation_i << " combined" << endl;
			for (auto i : combinedPopulation) {
				cout << i.solution << " penalty " << i.penalty << endl;
			}
			coutMutex.unlock();
*/
			auto nextPopulationLeftHalf = ScatterSearchPopulation<Individual*>(combinedPopulation, currentPopulationLeftHalf.elite.size()+currentPopulationRightHalf.elite.size(), currentPopulationLeftHalf.diverse.size()+currentPopulationRightHalf.diverse.size());

			auto nextPopulationRightHalfSlice = PopulationSlice<Individual*>(currentPopulationRightHalf.total.end(), currentPopulationRightHalf.total.end()+combinedPopulation.size());
			auto nextPopulationRightHalf = ScatterSearchPopulation<Individual*>(nextPopulationRightHalfSlice, nextPopulationLeftHalf.elite.size(), nextPopulationLeftHalf.diverse.size());

			bottomUpTreeDiversify(graph, nextPopulationLeftHalf, nextPopulationRightHalf, mutex, currentPopulation_i, combinationsCount*2, numberOfThreads);
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

void arrangePopulation (PopulationInterface<Individual> &totalPopulation, PopulationInterface<Individual*> &population, unsigned thread_i) {

	auto totalPopulationBegin = population.size()*thread_i;
	auto totalPopulationEnd = totalPopulationBegin+population.size();
	size_t total_i, population_i;

	for (total_i = totalPopulationBegin, population_i = 0; total_i < totalPopulationEnd; total_i++, population_i++) {
		population[population_i] = &totalPopulation[total_i];
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

	Population<Individual> totalPopulation(scatterSearchPopulationSize(elitePopulationSize, diversePopulationSize), graph.getNumberOfVertices());
	ScatterSearchPopulation<Individual> totalSubdividedPopulation(totalPopulation, elitePopulationSize, diversePopulationSize);

   	Population<Individual*> populationArrangement(totalPopulation.size());	

	vector<ScatterSearchPopulation<Individual*>> population(numberOfThreads);

	const auto threadPopulationSize = totalPopulation.size()/numberOfThreads;
	const auto threadElitePopulationSize = elitePopulationSize/numberOfThreads;
	const auto threadDiversePopulationSize = diversePopulationSize/numberOfThreads;
	const auto threadCandidatePopulationSize = threadElitePopulationSize+threadDiversePopulationSize/2;

	metrics.executionBegin = chrono::high_resolution_clock::now();

	for_each_thread (numberOfThreads) {

		auto elitePopulationBegin = threadElitePopulationSize*thread_i;
		auto elitePopulationEnd = threadElitePopulationSize*(thread_i+1);
		auto diversePopulationBegin = elitePopulationSize + threadDiversePopulationSize*thread_i;
		auto diversePopulationEnd = elitePopulationSize + threadDiversePopulationSize*(thread_i+1);

		auto threadPopulation = populationArrangement.slice(threadPopulationSize*thread_i, threadPopulationSize*(thread_i+1));
		population[thread_i] = ScatterSearchPopulation<Individual*>(threadPopulation, threadElitePopulationSize, threadDiversePopulationSize);
		
		for (auto i = elitePopulationBegin; i < elitePopulationEnd; i++) {
			totalPopulation[i].solution = constructHeuristicSolution(graph);
			totalPopulation[i].solution = localSearchHeuristic(graph, totalPopulation[i].solution, eliteLocalSearchStopFunction);
			totalPopulation[i].penalty = graph.totalPenalty(totalPopulation[i].solution);
		}
		
		for (auto i = diversePopulationBegin; i < diversePopulationEnd; i++) {
			totalPopulation[i].solution = constructHeuristicSolution(graph);
			totalPopulation[i].penalty = graph.totalPenalty(totalPopulation[i].solution);
		}

		arrangePopulation(totalSubdividedPopulation.elite, population[thread_i].elite, thread_i);
		arrangePopulation(totalSubdividedPopulation.diverse, population[thread_i].diverse, thread_i);
		arrangePopulation(totalSubdividedPopulation.candidate, population[thread_i].candidate, thread_i);

	} end_for_each_thread;

	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	while (stopFunction(metrics)) {
		/*
			cout << " total population" << endl;
			for (auto i : totalPopulation) {
				cout << i.solution << " penalty " << i.penalty << endl;
			}
			coutMutex.unlock();
*/

		vector<mutex> populationMutex(numberOfThreads);
		for_each_thread (numberOfThreads) {

/*
			cout << " arrangement " << thread_i << endl;
			for (auto i : population[thread_i]) {
				cout << i->solution << " penalty " << i->penalty << endl;
			}
			coutMutex.unlock();
*/

			random_device seeder;
			mt19937 randomEngine(seeder());

			Individual *individual1, *individual2;

			populationMutex[thread_i].lock();

				shuffle(population[thread_i].reference.begin(), population[thread_i].reference.end(), randomEngine);

				for (size_t i = 0; i < population[thread_i].candidate.size(); i++) {

					individual1 = population[thread_i].reference[i*2];
					individual2 = population[thread_i].reference[i*2+1];

					population[thread_i].candidate[i]->solution = combinationMethod(graph, &individual1->solution, &individual2->solution);
					population[thread_i].candidate[i]->solution = localSearchHeuristic(graph, population[thread_i].candidate[i]->solution, diverseLocalSearchStopFunction);
					population[thread_i].candidate[i]->penalty = graph.totalPenalty(population[thread_i].candidate[i]->solution);

				}

				sort(population[thread_i].total.begin(), population[thread_i].total.end());

				diversify(graph, population[thread_i]);

				if ((thread_i&1) == 0) {
					bottomUpTreeDiversify(graph, population[thread_i], population[thread_i+1], populationMutex, thread_i, 1, numberOfThreads);
				}

			populationMutex[thread_i].unlock();

		} end_for_each_thread;

		metrics.numberOfIterations++;

	}

	return min_element(totalPopulation.begin(), totalPopulation.begin()+elitePopulationSize)->solution;

}
