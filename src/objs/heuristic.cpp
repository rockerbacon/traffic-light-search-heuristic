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
	TimeUnit infinite = numeric_limits<TimeUnit>::max();
	size_t i;
	TimeUnit candidateTimingVertex1, candidateTimingVertex2, penalty;
	Solution solution(graph.getNumberOfVertices());

	for (Vertex i = 0; i < graph.getNumberOfVertices(); i++) {
		unvisitedVertices.push_back(i);
	}
	shuffle(unvisitedVertices.begin(), unvisitedVertices.end(), randomEngine);

	while (unvisitedVertices.size() > 0) {

		vertex1 = unvisitedVertices.back();
		unvisitedVertices.pop_back();

		//pegar vizinho aleatorio
		neighborhood = &graph.neighborsOf(vertex1);
		edgePicker = uniform_int_distribution<Vertex>(0, neighborhood->size()-1);
		neighborhoodIterator = neighborhood->cbegin();
		advance(neighborhoodIterator, edgePicker(randomEngine));
		vertex2 = neighborhoodIterator->first;

		for (i = 0; i < 2*numberOfTuplesToTestPerIteration; i++) {
			candidateTimings[i] = timingPicker(randomEngine);
		}

		bestPenalty = infinite;
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

Solution traffic::localSearchHeuristic(const Graph& graph, const Solution& initialSolution, const std::function<bool(const HeuristicMetrics&)>& stopCriteriaNotMet) {
	Solution solution(initialSolution);
	TimeUnit currentTiming, currentPenalty;
	TimeUnit perturbationTiming, perturbationPenalty;
	Vertex vertex;
	bool iterationHadNoImprovement;
	HeuristicMetrics metrics;

	random_device seeder;
	mt19937 randomEngine(seeder());
	uniform_int_distribution<Vertex> vertexPicker(0, graph.getNumberOfVertices()-1);
	uniform_int_distribution<TimeUnit> timingPicker(0, graph.getCycle()-1);

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
		} else {
			solution.setTiming(vertex, currentTiming);
		}

		metrics.numberOfIterations++;
		if (iterationHadNoImprovement) {
			metrics.numberOfIterationsWithoutImprovement++;
		} else {
			metrics.numberOfIterationsWithoutImprovement = 0;
		}
	}
	return solution;
}

template<typename T>
class VectorSlice {
	private:
		typename vector<T>::iterator iteratorToBegin;
		typename vector<T>::iterator iteratorToEnd;
		size_t rangeSize;
	public:
		VectorSlice (void) {}
		VectorSlice (vector<T>& vec, size_t begin, size_t end) {
			this->iteratorToBegin = vec.begin()+begin;
			this->iteratorToEnd = vec.begin()+end;
			this->rangeSize = end - begin;
		}

		typename vector<T>::iterator begin (void) {
			return this->iteratorToBegin;
		}

		typename vector<T>::iterator end (void) {
			return this->iteratorToEnd;
		}

		T& operator[] (size_t index) {
			return *(this->iteratorToBegin+index);
		}

		const T& operator[] (size_t index) const {
			return *(this->iteratorToBegin+index);
		}

		size_t size (void) const {
			return this->rangeSize;
		}
};

void fillWithMostDiverseCandidates (const Graph& graph, vector<const Solution*>& referenceSet, const VectorSlice<Solution>& diverseSet, size_t sizeToFill) {
	TimeUnit greatestDistance, smallestDistance, distance;
	TimeUnit	infinite = numeric_limits<TimeUnit>::max(),
				minusInfinite = numeric_limits<TimeUnit>::min();
	size_t chosenSolutionIndex;

	sizeToFill -= referenceSet.size();

	greatestDistance = minusInfinite;
	for (size_t i = 0; i < sizeToFill; i++) {
		for (size_t j = 0; j < diverseSet.size(); j++) {
			smallestDistance = infinite;

			for (auto referenceSolution : referenceSet) {
				distance = traffic::distance(graph, diverseSet[j], *referenceSolution);
				if (distance < smallestDistance) {
					smallestDistance = distance;
				}
			}

			if (smallestDistance > greatestDistance) {
				greatestDistance = smallestDistance;
				chosenSolutionIndex = j;
			}
		}
		referenceSet.push_back(&diverseSet[chosenSolutionIndex]);
	}
}


Solution combine (const Graph& graph, const Solution *a, const Solution *b) {
	Solution r(graph.getNumberOfVertices());
	for (Vertex i = 0; i < graph.getNumberOfVertices()/2; i++) {
		r.setTiming(i, a->getTiming(i));
	}
	for (Vertex i = graph.getNumberOfVertices()/2; i < graph.getNumberOfVertices(); i++) {
		r.setTiming(i, b->getTiming(i));
	}
	return r;
}

Solution traffic::populationalHeuristic(const Graph& graph, size_t elitePopulationSize, size_t diversePopulationSize, const function<bool(const HeuristicMetrics&)>& stopCriteriaNotMet) {
	vector<Solution> population;
	vector<const Solution*> referenceSet;
	VectorSlice<Solution> eliteSet, diverseSet, candidateSet;
	size_t	livePopulationSize = elitePopulationSize+diversePopulationSize,
			totalPopulationSize = livePopulationSize + livePopulationSize/2;
	const Solution *solution1, *solution2;
	size_t i;
	HeuristicMetrics metrics;
	random_device seeder;
	mt19937 randomEngine(seeder());
	uniform_int_distribution<size_t> solutionPicker(0, livePopulationSize-1);

	if (livePopulationSize&2) {
		throw invalid_argument("elitePopulationSize+diversePopulationSize must be an even number");
	}

	population.reserve(totalPopulationSize);
	referenceSet.reserve(livePopulationSize);

	while (population.size() < livePopulationSize) {
		Solution constructedSolution = localSearchHeuristic(graph, constructHeuristicSolution(graph), stop_criteria::numberOfIterations(500));
		population.push_back(constructedSolution);
		referenceSet.push_back(&population.back());
	}
	eliteSet = VectorSlice<Solution>(population, 0, elitePopulationSize);
	candidateSet = VectorSlice<Solution>(population, livePopulationSize, totalPopulationSize);
	diverseSet = VectorSlice<Solution>(population, elitePopulationSize, totalPopulationSize);

	sort(population.begin(), population.end(), [&](const Solution& a, const Solution& b) -> bool {
		return graph.totalPenalty(a) < graph.totalPenalty(b);
	});

	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	while (stopCriteriaNotMet(metrics)) {

		for (i = 0; i < candidateSet.size(); i++) {

			solution1 = referenceSet[i*2];
			solution2 = referenceSet[i*2+1];

			candidateSet[i] = combine(graph, solution1, solution2);
			candidateSet[i] = localSearchHeuristic(graph, candidateSet[i], stop_criteria::numberOfIterations(500));
		}

		sort(population.begin(), population.end(), [&](const Solution& a, const Solution &b) -> bool {
			return graph.totalPenalty(a) < graph.totalPenalty(b);
		});

		referenceSet.clear();
		for (i = 0; i < elitePopulationSize; i++) {
			referenceSet.push_back(&eliteSet[i]);
		}
		fillWithMostDiverseCandidates(graph, referenceSet, diverseSet, totalPopulationSize);

		metrics.numberOfIterations++;
	}
	return population[0];
}

function<bool(const HeuristicMetrics&)> stop_criteria::numberOfIterations(unsigned numberOfIterationsToStop) {
	return [=](const HeuristicMetrics& metrics) -> bool {
		return metrics.numberOfIterations < numberOfIterationsToStop;
	};
}

function<bool(const HeuristicMetrics&)> stop_criteria::numberOfIterationsWithoutImprovement(unsigned numberOfIterationsToStop) {
	return [=](const HeuristicMetrics& metrics) -> bool {
		return metrics.numberOfIterationsWithoutImprovement < numberOfIterationsToStop;
	};
}
