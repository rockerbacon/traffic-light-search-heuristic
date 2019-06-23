#include "heuristic.h"

#include <vector>
#include <random>
#include <chrono>
#include <limits>
#include <algorithm>
#include <queue>

#include <iostream>

using namespace traffic;
using namespace std;

Solution traffic::constructRandomSolution (const Graph& graph) {

	random_device seeder;
	mt19937 randomEngine(seeder());
	uniform_int_distribution<TimeUnit> timingPicker(0, graph.getCycle()-1);
	Solution solution(graph.getNumberOfVertices());

	for (Vertex v = 0; v < graph.getNumberOfVertices(); v++) {
		solution.setTiming(v, timingPicker(randomEngine));
	}

	return solution;

}

Solution traffic::constructHeuristicSolution (const Graph& graph, unsigned char numberOfTuplesToTestPerIteration) {
	vector<Vertex> unvisitedVertices(graph.getNumberOfVertices());
	const unordered_map<Vertex, Weight>* neighborhood;
	unordered_map<Vertex, Weight>::const_iterator neighborhoodIterator;
	TimeUnit *candidateTimings = new TimeUnit[2*numberOfTuplesToTestPerIteration];
	Vertex vertex1, vertex2;
	TimeUnit bestVertex1Timing, bestVertex2Timing, bestPenalty;
	random_device seeder;
	mt19937 randomEngine(seeder());
	uniform_int_distribution<Vertex> edgePicker;
	uniform_int_distribution<TimeUnit> timingPicker(0, graph.getCycle()-1);
	TimeUnit maxPenaltyPossible = numeric_limits<TimeUnit>::max();
	size_t i;
	TimeUnit candidateTimingVertex1, candidateTimingVertex2, penalty;
	Solution solution(graph.getNumberOfVertices());

	for (Vertex i = 0; i < graph.getNumberOfVertices(); i++) {
		solution.setTiming(i, 0);
		unvisitedVertices.push_back(i);
	}
	shuffle(unvisitedVertices.begin(), unvisitedVertices.end(), randomEngine);

	randomEngine = mt19937(seeder());
	while (unvisitedVertices.size() > 0) {

		vertex1 = unvisitedVertices.back();
		unvisitedVertices.pop_back();

		neighborhood = &graph.neighborsOf(vertex1);
		edgePicker = uniform_int_distribution<Vertex>(0, neighborhood->size()-1);
		neighborhoodIterator = neighborhood->cbegin();
		advance(neighborhoodIterator, edgePicker(randomEngine));
		vertex2 = neighborhoodIterator->first;

		for (i = 0; i < 2*numberOfTuplesToTestPerIteration; i++) {
			candidateTimings[i] = timingPicker(randomEngine);
		}

		bestPenalty = maxPenaltyPossible;
		for (i = 0; i < numberOfTuplesToTestPerIteration; i++) {
			candidateTimingVertex1 = candidateTimings[i*2];
			candidateTimingVertex2 = candidateTimings[i*2+1];

			solution.setTiming(vertex1, candidateTimingVertex1);
			solution.setTiming(vertex2, candidateTimingVertex2);

			penalty = graph.vertexPenalty(vertex1, solution) + graph.vertexPenalty(vertex2, solution);
			if (penalty < bestPenalty) {
				bestPenalty = penalty;
				bestVertex1Timing = candidateTimingVertex1;
				bestVertex2Timing = candidateTimingVertex2;
			}
		}

		solution.setTiming(vertex1, bestVertex1Timing);
		solution.setTiming(vertex2, bestVertex2Timing);

	}

	delete [] candidateTimings;

	return solution;
}

TimeUnit traffic::distance(const Graph& graph, const Solution& a, const Solution& b) {
	TimeUnit totalDistance, clockwiseVertexDistance, counterClockwiseVertexDistance;
	totalDistance = 0;
	for (Vertex v = 0; v < graph.getNumberOfVertices(); v++) {
		clockwiseVertexDistance = abs(a.getTiming(v) - b.getTiming(v));
		counterClockwiseVertexDistance = graph.getCycle() - clockwiseVertexDistance;
		if (clockwiseVertexDistance < counterClockwiseVertexDistance) {
			totalDistance += clockwiseVertexDistance;
		} else {
			totalDistance += counterClockwiseVertexDistance;
		}
	}
	return totalDistance;
}

struct Perturbation {
	TimeUnit timing;
	TimeUnit penalty;
};

Solution traffic::localSearchHeuristic(const Graph& graph, const Solution& initialSolution, unsigned numberOfPerturbations, size_t perturbationHistorySize) {
	Solution bestSolution(initialSolution), solution(initialSolution);
	TimeUnit bestTimingForVertex, bestPenalty;
	unordered_set<Vertex> perturbationHistory, perturbationHistoryCompliment; // TODO optimize space usage
	queue<decltype(perturbationHistory)::const_iterator> perturbationHistoryRemovalQueue;
	decltype(perturbationHistory)::const_iterator perturbationIterator;
	Vertex vertex;
	Perturbation *perturbations;
	size_t i, numberOfIterations, vertexIndex;
	TimeUnit rouletteMax, roulette, rouletteTarget;
	bool stillPickingSolution;

	random_device seeder;
	mt19937 randomEngine(seeder());
	uniform_int_distribution<Vertex> vertexIndexPicker;
	uniform_int_distribution<TimeUnit> timingPicker(0, graph.getCycle()-1), roulettePicker;

	perturbations = new Perturbation[numberOfPerturbations+1];
	for (vertex = 0; vertex < graph.getNumberOfVertices(); vertex++) {
		perturbationHistoryCompliment.emplace(vertex);
	}

	numberOfIterations = 0;
	// TODO define stop criteria
	while (numberOfIterations < 10) {
		vertexIndexPicker = uniform_int_distribution<Vertex>(0, perturbationHistoryCompliment.size()-1);
		vertexIndex = vertexIndexPicker(randomEngine);
		perturbationIterator = perturbationHistoryCompliment.cbegin();
		advance(perturbationIterator, vertexIndex);
		vertex = *perturbationIterator;
		if (perturbationHistory.size() == perturbationHistorySize) {
			perturbationIterator = perturbationHistoryRemovalQueue.front();
			perturbationHistoryRemovalQueue.pop();
			perturbationHistory.erase(perturbationIterator);

			perturbationHistoryCompliment.emplace(*perturbationIterator);
		}
		perturbationHistoryCompliment.erase(vertex);
		perturbationIterator = perturbationHistory.emplace(vertex).first;
		perturbationHistoryRemovalQueue.push(perturbationIterator);
		bestTimingForVertex = bestSolution.getTiming(vertex);
		bestPenalty = graph.vertexPenalty(vertex, bestSolution);
		perturbations[0].timing = bestTimingForVertex;
		perturbations[0].penalty = bestPenalty;
		rouletteMax = bestPenalty;
		for (i = 1; i <= numberOfPerturbations; i++) {
			perturbations[i].timing = timingPicker(randomEngine);
			solution.setTiming(vertex, perturbations[i].timing);
			perturbations[i].penalty = graph.vertexPenalty(vertex, solution);
			rouletteMax += perturbations[i].penalty;

			if (perturbations[i].penalty < bestPenalty) {
				bestSolution.setTiming(vertex, perturbations[i].timing);
				bestPenalty = perturbations[i].penalty;
			}
		}
		// TODO review roulette
		sort(perturbations, perturbations + numberOfPerturbations+1, [](const Perturbation& a, const Perturbation &b) -> bool {
			return a.penalty < b.penalty;
		});
		stillPickingSolution = true;
		roulettePicker = uniform_int_distribution<TimeUnit>(0, rouletteMax-1);
		rouletteTarget = roulettePicker(randomEngine);
		roulette = 0;
		i = 0;
		while (stillPickingSolution && i <= numberOfPerturbations) {
			roulette += perturbations[i].penalty;
			if (rouletteTarget < roulette) {
				solution.setTiming(vertex, perturbations[i].timing);
				stillPickingSolution = false;
			}
			i++;
		}

		numberOfIterations++;
	}

	delete [] perturbations;

	return bestSolution;
}
