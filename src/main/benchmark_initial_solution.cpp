#include <iostream>
#include <string>
#include <cstring>

#include "heuristic/heuristic.h"
#include "assertions/benchmark.h"

#define DEFAULT_NUMBER_OF_VERTICES 500
#define DEFAULT_NUMBER_OF_RUNS 100
#define DEFAULT_CYCLE 24
#define DEFAULT_MIN_VERTEX_DEGREE 4
#define DEFAULT_MAX_VERTEX_DEGREE 10

#define WRONG_ARGUMENTS_EXIT_CODE 1

using namespace std;
using namespace traffic;
using namespace benchmark;

void setupExecutionParameters (int argc, char** argv, size_t &numberOfVertices, size_t &minVertexDegree, size_t &maxVertexDegree, unsigned &numberOfRuns, TimeUnit &cycle) {
	numberOfVertices = DEFAULT_NUMBER_OF_VERTICES;
	minVertexDegree = DEFAULT_MIN_VERTEX_DEGREE;
	maxVertexDegree = DEFAULT_MAX_VERTEX_DEGREE;
	numberOfRuns = DEFAULT_NUMBER_OF_RUNS;
	cycle = DEFAULT_CYCLE;

	if (argc > 1) {
		int i = 1;
		while (i < argc) {
			if (strcmp(argv[i], "--vertices") == 0) {
				i++;
				if (i >= argc) {
					cout << "--vertices argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				numberOfVertices = atoi(argv[i]);
				if (numberOfVertices == 0) {
					cout << "--vertices argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
			} else if (strcmp(argv[i], "--runs") == 0) {
				i++;
				if (i >= argc) {
					cout << "--runs argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				numberOfRuns = atoi(argv[i]);
				if (numberOfRuns == 0) {
					cout << "--runs argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

			} else if (strcmp(argv[i], "--cycle") == 0) {

				i++;
				if (i >= argc) {
					cout << "--cycle argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				cycle = atoi(argv[i]);
				if (numberOfRuns == 0) {
					cout << "--cycle argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

			} else if (strcmp(argv[i], "--minVertexDegree") == 0) {

				i++;
				if (i >= argc) {
					cout << "--minVertexDegree argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				minVertexDegree = atoi(argv[i]);
				if (numberOfRuns == 0) {
					cout << "--minVertexDegree argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

			} else if (strcmp(argv[i], "--maxVertexDegree") == 0) {

				i++;
				if (i >= argc) {
					cout << "--maxVertexDegree argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				maxVertexDegree = atoi(argv[i]);
				if (numberOfRuns == 0) {
					cout << "--maxVertexDegree argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

			} else {
				cout << "unknown argument " << argv[i] << endl;
				exit(WRONG_ARGUMENTS_EXIT_CODE);
			}

			i++;

		}
	}
}

double iterativeVariety (const Graph &graph, const list<Solution>& previousSolutions, const Solution &newSolution) {
	double variety;	
	int j;
	list<Solution>::const_iterator it;

	variety = 0;
	for (j = 0, it = previousSolutions.begin(); it != previousSolutions.end(); it++, j++) {
		variety = (variety*j + distance(graph, *it, newSolution))/(j+1);
	}

	return variety;
}

int main (int argc, char** argv) {

	GraphBuilder *graphBuilder;
	Graph *graph;
	size_t numberOfVertices, minVertexDegree, maxVertexDegree;
	unsigned numberOfRuns;
	double randomVariety, heuristicVariety;
	double randomPenalty, heuristicPenalty;
	double lowerBound;
	TimeUnit cycle;
	list<Solution> randomSolutions, heuristicSolutions;
	double varietyFactor, penaltyFactor;
	chrono::high_resolution_clock::duration randomTime, heuristicTime;
	chrono::high_resolution_clock::time_point beginTime;
	double lowerBoundRandomFactor, lowerBoundHeuristicFactor;
	Solution solution;
	unsigned i, j;

	setupExecutionParameters(argc, argv, numberOfVertices, minVertexDegree, maxVertexDegree, numberOfRuns, cycle);

	register_observer(new TerminalObserver());

	observe_variable("Lower bound", lowerBound, observation_mode::AVERAGE_VALUE);
	observe_variable("Random construction variety", randomVariety, observation_mode::AVERAGE_VALUE);
	observe_variable("Random construction penalty", randomPenalty, observation_mode::AVERAGE_VALUE);
	observe_variable("Random construction time", randomTime, observation_mode::AVERAGE_VALUE);
	observe_variable("Random/Lower bound factor", lowerBoundRandomFactor, observation_mode::AVERAGE_VALUE);
	observe_variable("Heuristic construction variety", heuristicVariety, observation_mode::AVERAGE_VALUE);
	observe_variable("Heuristic construction penalty", heuristicPenalty, observation_mode::AVERAGE_VALUE);
	observe_variable("Heuristic construction time", heuristicTime, observation_mode::AVERAGE_VALUE);
	observe_variable("Heuristic/Lower bound factor", lowerBoundHeuristicFactor, observation_mode::AVERAGE_VALUE);
	observe_variable("Heuristic/Random variety factor", varietyFactor, observation_mode::AVERAGE_VALUE);
	observe_variable("Heuristic/Random penaltyFactor", penaltyFactor, observation_mode::AVERAGE_VALUE);

	benchmark("initial solution construction", numberOfRuns) {
		graphBuilder = new GraphBuilder(numberOfVertices, minVertexDegree, maxVertexDegree, 1, cycle-1);
		graphBuilder->withCycle(cycle);
		graph = graphBuilder->buildAsAdjacencyList();

		lowerBound = graph->lowerBound();

		beginTime = chrono::high_resolution_clock::now();
		solution = constructRandomSolution(*graph);

		randomTime = chrono::high_resolution_clock::now() - beginTime;
		randomPenalty = graph->totalPenalty(solution);
		lowerBoundRandomFactor = randomPenalty/lowerBound;
		randomVariety = iterativeVariety(*graph, randomSolutions, solution);

		randomSolutions.push_back(solution);

		beginTime = chrono::high_resolution_clock::now();
		solution = constructHeuristicSolution(*graph);

		heuristicTime = chrono::high_resolution_clock::now() - beginTime;
		heuristicPenalty = graph->totalPenalty(solution);
		lowerBoundHeuristicFactor = heuristicPenalty/lowerBound;
		heuristicVariety = iterativeVariety(*graph, heuristicSolutions, solution);

		heuristicSolutions.push_back(solution);

		if (benchmark::current_run > 1) {
			varietyFactor = heuristicVariety/randomVariety;
		} else {
			varietyFactor = 1;
		}
		penaltyFactor = heuristicPenalty/randomPenalty;

		delete graph;
		delete graphBuilder;
	} end_benchmark;

	delete_observers();

	return 0;
}
