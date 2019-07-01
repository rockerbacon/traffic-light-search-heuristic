#include "heuristic.h"

#include <random>
#include <algorithm>
#include <queue>

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

void fillWithMostDiverseCandidates (vector<Solution>& output, const vector<Solution>& candidates, size_t sizeToFill) {
	sizeToFill -= output.size();
	for (size_t i = 0; i < sizeToFill; i++) {
		output.push_back(candidates[i]);
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
	vector<Solution> initialPopulation, refinedPopulation, elitePopulation, referenceSet, candidateSet;
	size_t i, j, totalPopulationSize;
	Solution constructedSolution;
	HeuristicMetrics metrics;
	random_device seeder;
	mt19937 randomEngine(seeder());
	Solution *solution1, *solution2, newCandidate;

	totalPopulationSize = elitePopulationSize+diversePopulationSize;
	if (totalPopulationSize&2) {
		throw invalid_argument("elitePopulationSize+diversePopulationSize must be an even number");
	}
	initialPopulation.reserve(totalPopulationSize);
	refinedPopulation.reserve(totalPopulationSize);
	elitePopulation.reserve(elitePopulationSize);
	referenceSet.reserve(totalPopulationSize);
	candidateSet.reserve(totalPopulationSize/2);

	while (initialPopulation.size() < totalPopulationSize) {
		constructedSolution = constructHeuristicSolution(graph);
		initialPopulation.push_back(constructedSolution);
		refinedPopulation.push_back(localSearchHeuristic(graph, constructedSolution, stop_criteria::numberOfIterations(500)));
	}

	sort(refinedPopulation.begin(), refinedPopulation.end(), [&](const Solution& a, const Solution& b) -> bool {
		return graph.totalPenalty(a) < graph.totalPenalty(b);
	});

	for (i = 0; i < elitePopulationSize; i++) {
		elitePopulation.push_back(refinedPopulation[i]);
		referenceSet.push_back(refinedPopulation[i]);
	}

	fillWithMostDiverseCandidates (referenceSet, initialPopulation, totalPopulationSize);

	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	while (stopCriteriaNotMet(metrics)) {
		shuffle(referenceSet.begin(), referenceSet.end(), randomEngine);
		for (i = 0; i < totalPopulationSize/2; i++) {
			solution1 = &referenceSet[i*2];
			solution2 = &referenceSet[i*2+1];
			newCandidate = combine(graph, solution1, solution2);
			newCandidate = localSearchHeuristic(graph, newCandidate, stop_criteria::numberOfIterations(500));
			candidateSet.push_back(newCandidate);
		}
		sort(candidateSet.begin(), candidateSet.end(), [&](const Solution& a, const Solution& b) -> bool {
			return graph.totalPenalty(a) < graph.totalPenalty(b);
		});

		referenceSet.clear();
		i = 0;
		j = 0;
		while(i+j < elitePopulationSize) {
			if (graph.totalPenalty(elitePopulation[i]) < graph.totalPenalty(candidateSet[j])) {
				referenceSet.push_back(elitePopulation[i]);
				i++;
			} else {
				referenceSet.push_back(candidateSet[j]);
				j++;
			}
		}

		elitePopulation.clear();
		for (i = 0; i < elitePopulationSize; i++) {
			elitePopulation.push_back(referenceSet[i]);
		}

		fillWithMostDiverseCandidates(candidateSet, initialPopulation, totalPopulationSize);
		fillWithMostDiverseCandidates(referenceSet, candidateSet, totalPopulationSize);
		candidateSet.clear();

		metrics.numberOfIterations++;
	}

	return elitePopulation[0];
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

Solution traffic::combineByBfs(const Graph& graph, const Solution *s1, const Solution *s2)
{	
	Vertex v = 0;
	unsigned i = 0, middle;
	size_t nVertices = graph.getNumberOfVertices();
	bool *visited = new bool[nVertices]{false};
	Solution solution(nVertices);
	queue<Vertex> q;
	q.push(v);

	middle = nVertices % 2? (nVertices / 2) + 1 : nVertices / 2;

	while(!q.empty())
	{
		v = q.front();
		q.pop();
		visited[v] = true;

		for(auto u : graph.neighborsOf(v))
		{
			if(!visited[u.first])
			{
				q.push(u.first);
			}
		}

		if(i < middle)
		{
			solution.setTiming(v, s1->getTiming(v));
		}
		else
		{
			solution.setTiming(v, s2->getTiming(v));
		}

		i++;
	}

	return solution;
}

/*
In our genetic algorithm, a chromosome represents a solution of traffic light setting on the traffic graph. The fitness of each chromosome is measured by the total penalty of vehicles on this traffic
light setting. The crossover is done by randomly selecting two chromosomes A = a1a2 · · · aN and B = b1b2 · · · bN from the population. Then, via exchanging information, A and B produce a new offspring 
C = c1c2 · · · cN . We choose a value k, where k is the half length of chromosome plus a random value p. In our method, p falls within the range from −2 to 2. If index i ≤ k, gene ci = ai. Otherwise, 
gene ci = bi . The mutation is done by randomly changing genes on the offspring C, with the mutation probability.

OBS.: 0.0 <= mutationProb <= 1.0
	  range do p não está fixo em -2 a 2, mas em -pRange a pRange
*/
Solution traffic::crossover(const Graph& graph, const Solution *a, const Solution *b, int pRange, double mutationProb)
{
	if(mutationProb > 1.0)
	{
		mutationProb = 1.0;
	}

	size_t nVertices = graph.getNumberOfVertices();
	pRange = abs(pRange);

	if(pRange > nVertices / 2)
	{
		pRange = nVertices / 2;
	}

	random_device seeder;
	mt19937 randomEngine(seeder());
	uniform_int_distribution<int> pPicker(-pRange, pRange);
	uniform_real_distribution<double> mutPicker(0.0, 1.0);
	uniform_int_distribution<TimeUnit> timingPicker(0, graph.getCycle()-1);

	Solution solution(nVertices);

	int p = pPicker(randomEngine);
	int k = nVertices / 2 + p;

	for(Vertex v = 0; v < nVertices; v++)
	{
		solution.setTiming(v, v <= k ? a->getTiming(v) : b->getTiming(v));
		
		if(mutPicker(randomEngine) <= mutationProb)
		{
			solution.setTiming(v, timingPicker(randomEngine));
		}
	}

	return solution;
}
