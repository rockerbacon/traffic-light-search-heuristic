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

Solution traffic::localSearchHeuristic(const Graph& graph, const Solution& initialSolution, const std::function<bool(const LocalSearchMetrics&)>& stopCriteriaNotMet) {
	Solution bestSolution(initialSolution), solution(initialSolution);
	TimeUnit bestTiming, bestPenalty;
	TimeUnit currentTiming, currentPenalty;
	TimeUnit perturbationTiming, perturbationPenalty;
	Vertex vertex;
	bool iterationHadNoImprovement;
	LocalSearchMetrics metrics;

	random_device seeder;
	mt19937 randomEngine(seeder());
	uniform_int_distribution<Vertex> vertexPicker(0, graph.getNumberOfVertices()-1);
	uniform_int_distribution<TimeUnit> timingPicker(0, graph.getCycle()-1), roulettePicker;
	uniform_real_distribution<double> probabilityPicker(0, 1);

	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	while (stopCriteriaNotMet(metrics)) {
		iterationHadNoImprovement = true;

		vertex = vertexPicker(randomEngine);

		currentTiming = solution.getTiming(vertex);
		currentPenalty = graph.vertexPenalty(vertex, solution);

		perturbationTiming = timingPicker(randomEngine);
		solution.setTiming(vertex, perturbationTiming);
		perturbationPenalty = graph.vertexPenalty(vertex, solution);

		if (perturbationPenalty < currentPenalty) {
			iterationHadNoImprovement = false;
		} else if (probabilityPicker(randomEngine) < 0.5) {
			solution.setTiming(vertex, currentTiming);
		}

		bestTiming = bestSolution.getTiming(vertex);
		bestPenalty = graph.vertexPenalty(vertex, bestSolution);

		bestSolution.setTiming(vertex, perturbationTiming);
		currentPenalty = graph.vertexPenalty(vertex, bestSolution);
		if (currentPenalty < bestPenalty) {
			bestTiming = perturbationTiming;
			bestPenalty = currentPenalty;
		} else {
			bestSolution.setTiming(vertex, bestTiming);
		}

		metrics.numberOfIterations++;
		if (iterationHadNoImprovement) {
			metrics.numberOfIterationsWithoutImprovement++;
		} else {
			metrics.numberOfIterationsWithoutImprovement = 0;
		}
	}

	return bestSolution;
}

function<bool(const LocalSearchMetrics&)> stop_criteria::numberOfIterations(unsigned numberOfIterationsToStop) {
	return [=](const LocalSearchMetrics& metrics) -> bool {
		return metrics.numberOfIterations < numberOfIterationsToStop;
	};
}

function<bool(const LocalSearchMetrics&)> stop_criteria::numberOfIterationsWithoutImprovement(unsigned numberOfIterationsToStop) {
	return [=](const LocalSearchMetrics& metrics) -> bool {
		return metrics.numberOfIterationsWithoutImprovement < numberOfIterationsToStop;
	};
}
