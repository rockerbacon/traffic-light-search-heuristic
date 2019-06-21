#include "heuristic.h"

#include <vector>
#include <random>
#include <chrono>
#include <limits>
#include <algorithm>

using namespace traffic;
using namespace std;

void traffic::constructRandomSolution (Graph& graph) {

	random_device seeder;
	mt19937 randomEngine(seeder());
	uniform_int_distribution<TimeUnit> timingPicker(0, graph.getCycle()-1);

	for (Vertex v = 0; v < graph.getNumberOfVertices(); v++) {
		graph.setTiming(v, timingPicker(randomEngine));
	}

}

void traffic::constructHeuristicSolution (Graph& graph, unsigned char numberOfTuplesToTestPerIteration) {
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

	for (Vertex i = 0; i < graph.getNumberOfVertices(); i++) {
		graph.setTiming(i, 0);
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

			graph.setTiming(vertex1, candidateTimingVertex1);
			graph.setTiming(vertex2, candidateTimingVertex2);

			penalty = graph.vertexPenalty(vertex1) + graph.vertexPenalty(vertex2);
			if (penalty < bestPenalty) {
				bestPenalty = penalty;
				bestVertex1Timing = candidateTimingVertex1;
				bestVertex2Timing = candidateTimingVertex2;
			}
		}

		graph.setTiming(vertex1, bestVertex1Timing);
		graph.setTiming(vertex2, bestVertex2Timing);

	}

	delete [] candidateTimings;
}
