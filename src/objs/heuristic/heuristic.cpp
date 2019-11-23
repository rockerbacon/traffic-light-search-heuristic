#include "heuristic.h"

#include <random>
#include <algorithm>
#include <queue>

using namespace traffic;
using namespace heuristic;
using namespace std;

Solution heuristic::constructRandomSolution (const Graph& graph) {

	random_device seeder;
	mt19937 randomEngine(seeder());
	uniform_int_distribution<TimeUnit> timingPicker(0, graph.getCycle()-1);
	Solution solution(graph.getNumberOfVertices());

	for (Vertex v = 0; v < graph.getNumberOfVertices(); v++) {
		solution.setTiming(v, timingPicker(randomEngine));
	}

	return solution;

}

Solution heuristic::constructHeuristicSolution (const Graph& graph, unsigned char numberOfTuplesToTestPerIteration) {
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

TimeUnit heuristic::distance(const Graph& graph, const Solution& a, const Solution& b) {
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

Solution heuristic::localSearchHeuristic(const Graph& graph, const Solution& initialSolution, const StopFunction &stopCriteriaNotMet) {
	Solution solution(initialSolution);
	TimeUnit currentTiming, currentPenalty;
	TimeUnit perturbationTiming, perturbationPenalty;
	Vertex vertex;
	bool iterationHadNoImprovement;
	Metrics metrics;

	random_device seeder;
	mt19937 randomEngine(seeder());
	uniform_int_distribution<Vertex> vertexPicker(0, graph.getNumberOfVertices()-1);
	uniform_int_distribution<TimeUnit> timingPicker(0, graph.getCycle()-1);

	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	metrics.executionBegin = chrono::high_resolution_clock::now();
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

StopFunction stop_function_factory::numberOfIterations(unsigned numberOfIterationsToStop) {
	return [=](const Metrics& metrics) -> bool {
		return metrics.numberOfIterations < numberOfIterationsToStop;
	};
}

StopFunction stop_function_factory::numberOfIterationsWithoutImprovement(unsigned numberOfIterationsToStop) {
	return [=](const Metrics& metrics) -> bool {
		return metrics.numberOfIterationsWithoutImprovement < numberOfIterationsToStop;
	};
}

CombinationMethod combination_method_factory::breadthFirstSearch (double mutationProbability) {
	return [mutationProbability](const Graph& graph, const Solution *s1, const Solution *s2) -> Solution {
		random_device seeder;
		mt19937 randomEngine(seeder());
		uniform_int_distribution<Vertex> vertexPicker(0, graph.getNumberOfVertices()-1);
		uniform_int_distribution<TimeUnit> timingPicker(0, graph.getCycle()-1);
		uniform_real_distribution<decltype(mutationProbability)> mutationPicker(0.0, 1.0);
		Vertex v = vertexPicker(randomEngine);
		unsigned i = 0, middle;
		size_t nVertices = graph.getNumberOfVertices();
		bool *visited = new bool[nVertices]{false};
		Solution solution(nVertices);
		queue<Vertex> q;
		q.push(v);

		middle = nVertices % 2 ? (nVertices / 2) + 1 : nVertices / 2;

		visited[v] = true;

		while(!q.empty())
		{
			v = q.front();
			q.pop();

			for(auto u : graph.neighborsOf(v))
			{
				if(!visited[u.first])
				{
					visited[u.first] = true;
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

		if (mutationPicker(randomEngine) <= mutationProbability) {
			auto vertex = vertexPicker(randomEngine);
			auto timing = timingPicker(randomEngine);
			solution.setTiming(vertex, timing);
		}

		delete [] visited;
		return solution;
	};
}

CombinationMethod combination_method_factory::crossover (double mutationProbability) {
	return [=](const Graph& graph, const Solution *a, const Solution *b) -> Solution {

		Vertex nVertices = graph.getNumberOfVertices();
		Vertex pRange = nVertices / 2;

		random_device seeder;
		mt19937 randomEngine(seeder());
		uniform_int_distribution<int> pPicker(-pRange, pRange);
		uniform_real_distribution<double> mutPicker(0.0, 1.0);
		uniform_int_distribution<TimeUnit> timingPicker(0, graph.getCycle()-1);

		Solution solution(nVertices);

		int p = pPicker(randomEngine);
		Vertex k = nVertices / 2 + p;

		for(Vertex v = 0; v < nVertices; v++)
		{
			solution.setTiming(v, v <= k ? a->getTiming(v) : b->getTiming(v));

			if(mutPicker(randomEngine) <= mutationProbability)
			{
				solution.setTiming(v, timingPicker(randomEngine));
			}
		}

		return solution;
	};
}

Solution heuristic::geneticAlgorithm(const Graph& graph, size_t populationSize, const StopFunction &stopFunction, const CombinationMethod &combinationMethod) {
	if(populationSize < 2)
	{
		throw invalid_argument("populationSize must be >= 2");
	}

	Solution bestSolution(graph.getNumberOfVertices()), aux;
	TimeUnit lowestPenalty = numeric_limits<TimeUnit>::max();
	random_device seeder;
	mt19937 randomEngine(seeder());
	uniform_int_distribution<size_t> tournamentPicker;
	vector<pair<Solution, TimeUnit>> population, parents;
	pair<Solution, TimeUnit> tournamentWinner, tournamentIndividual;
	unsigned replaceSize = populationSize * 1.0, tournamentSize = 0.4 * populationSize;

	Metrics metrics;
	bool iterationHadNoImprovement;

	if(tournamentSize < 2)
	{
		tournamentSize = 2;
	}

	if(replaceSize < 2)
	{
		replaceSize = 2;
	}

	population.reserve(populationSize + replaceSize);
	parents.reserve(replaceSize);
	tournamentPicker.param(std::uniform_int_distribution<size_t>::param_type(0, populationSize - 1));

	metrics.executionBegin = chrono::high_resolution_clock::now();
	for(size_t i = 0; i < populationSize; i++)
	{
		aux = constructHeuristicSolution(graph);
		population.push_back(make_pair(aux, graph.totalPenalty(aux)));

		if(population[i].second < lowestPenalty)
		{
			bestSolution = population[i].first;
			lowestPenalty = population[i].second;
		}
	}

	metrics.numberOfIterations = 0;
	metrics.numberOfIterationsWithoutImprovement = 0;
	while(stopFunction(metrics))
	{
		iterationHadNoImprovement = true;
		for(unsigned selectedParents = 0; selectedParents < replaceSize; selectedParents++)
		{
			size_t random = tournamentPicker(randomEngine);
			tournamentWinner = population[random];

			for(unsigned j = 0; j < tournamentSize - 1; j++)
			{
				random = tournamentPicker(randomEngine);
				tournamentIndividual = population[random];

				if(tournamentIndividual.second < tournamentWinner.second)
				{
					tournamentWinner = tournamentIndividual;
				}
			}

			parents.push_back(tournamentWinner);
		}

		std::sort(parents.begin(), parents.end(), [](auto &a, auto &b) {
    		return a.second < b.second;
		});

		for(size_t j = 0; j < replaceSize; j++)
		{
			aux = combinationMethod(graph, &parents[j].first, &parents[(j+1) % replaceSize].first);
			population.push_back(make_pair(aux, graph.totalPenalty(aux)));
		}

		std::sort(population.begin(), population.end(), [](auto &a, auto &b) {
    		return a.second < b.second;
		});

		population.erase(population.end() - 1 - replaceSize, population.end() - 1);

		if(population[0].second < lowestPenalty)
		{
			lowestPenalty = population[0].second;
			bestSolution = population[0].first;
			iterationHadNoImprovement = false;
		}

		parents.clear();

		metrics.numberOfIterations++;
		if (iterationHadNoImprovement) {
			metrics.numberOfIterationsWithoutImprovement++;
		} else {
			metrics.numberOfIterationsWithoutImprovement = 0;
		}
	}

	return bestSolution;
}
