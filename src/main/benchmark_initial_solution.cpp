#include <iostream>
#include <string>
#include <cstring>

#include "heuristic.h"
#include "benchmark.h"

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

int main (int argc, char** argv) {

	GraphBuilder *graphBuilder;
	Graph *graph;
	list<Observer*> observers;
	TerminalObserver *terminalObserver;
	size_t numberOfVertices, minVertexDegree, maxVertexDegree;
	unsigned numberOfRuns;
	TimeUnit variety;
	double avgRandomVariety, avgHeuristicVariety;
	double avgRandomPenalty, avgHeuristicPenalty;
	double avgLowerBound;
	TimeUnit cycle;
	list<Solution> randomSolutions, heuristicSolutions;
	double varietyFactor, penaltyFactor;
	decltype(randomSolutions)::iterator it;
	chrono::nanoseconds avgRandomTime, avgHeuristicTime;
	chrono::high_resolution_clock::time_point beginTime;
	string formatedAvgRandomTime, formatedAvgHeuristicTime;
	double lowerBoundRandomFactor, lowerBoundHeuristicFactor;
	Solution solution;
	unsigned i, j;

	setupExecutionParameters(argc, argv, numberOfVertices, minVertexDegree, maxVertexDegree, numberOfRuns, cycle);

	terminalObserver = new TerminalObserver();
	terminalObserver->observeVariable("Lower bound", avgLowerBound);
	terminalObserver->observeVariable("Random construction variety", avgRandomVariety);
	terminalObserver->observeVariable("Random construction penalty", avgRandomPenalty);
	terminalObserver->observeVariable("Random construction time", formatedAvgRandomTime);
	terminalObserver->observeVariable("Random/Lower bound factor", lowerBoundRandomFactor);
	terminalObserver->observeVariable("Heuristic construction variety", avgHeuristicVariety);
	terminalObserver->observeVariable("Heuristic construction penalty", avgHeuristicPenalty);
	terminalObserver->observeVariable("Heuristic construction time", formatedAvgHeuristicTime);
	terminalObserver->observeVariable("Heuristic/Lower bound factor", lowerBoundHeuristicFactor);
	terminalObserver->observeVariable("Heuristic/Random variety factor", varietyFactor);
	terminalObserver->observeVariable("Heuristic/Random penaltyFactor", penaltyFactor);
	observers.push_back(terminalObserver);

	avgLowerBound = 0;
	avgRandomVariety = 0;
	avgRandomPenalty = 0;
	avgRandomTime = chrono::high_resolution_clock::duration(0);
	avgHeuristicVariety = 0;
	avgHeuristicPenalty = 0;
	avgHeuristicTime = chrono::high_resolution_clock::duration(0);
	for (auto o : observers) o->notifyBenchmarkBegun("initial solution construction", numberOfRuns);
	for (i = 0; i < numberOfRuns; i++) {
		graphBuilder = new GraphBuilder(numberOfVertices, minVertexDegree, maxVertexDegree, 1, cycle-1);
		graphBuilder->withCycle(cycle);
		graph = graphBuilder->buildAsAdjacencyList();

		for (auto o : observers) o->notifyRunBegun();

		beginTime = chrono::high_resolution_clock::now();
		solution = constructRandomSolution(*graph);

		avgRandomTime = (avgRandomTime*i + chrono::high_resolution_clock::now() - beginTime)/(i+1);
		avgRandomPenalty = (avgRandomPenalty*i +graph->totalPenalty(solution))/(i+1);
		avgLowerBound = (avgLowerBound*i + graph->lowerBound())/(i+1);

		if (i > 0) {
			variety = 0;
			for (j = 0, it = randomSolutions.begin(); it != randomSolutions.end(); it++, j++) {
				variety = (variety*j + distance(*graph, *it, solution))/(j+1);
			}
			avgRandomVariety = (avgRandomVariety*i + variety)/(i+1);
		}
		randomSolutions.push_back(solution);

		formatedAvgRandomTime = format_chrono_duration(avgRandomTime);
		lowerBoundRandomFactor = avgRandomPenalty/avgLowerBound;

		beginTime = chrono::high_resolution_clock::now();
		solution = constructHeuristicSolution(*graph);

		avgHeuristicTime = (avgHeuristicTime*i + chrono::high_resolution_clock::now() - beginTime)/(i+1);
		avgHeuristicPenalty = (avgHeuristicPenalty*i + graph->totalPenalty(solution))/(i+1);

		if (i > 0) {
			variety = 0;
			for (j = 0, it = heuristicSolutions.begin(); it != heuristicSolutions.end(); it++, j++) {
				variety = (variety*j + distance(*graph, *it, solution))/(j+1);
			}
			avgHeuristicVariety = (avgHeuristicVariety*i + variety)/(i+1);
		}
		heuristicSolutions.push_back(solution);

		formatedAvgHeuristicTime = format_chrono_duration(avgHeuristicTime);
		lowerBoundHeuristicFactor = avgHeuristicPenalty/avgLowerBound;

		varietyFactor = avgHeuristicVariety/avgRandomVariety;
		penaltyFactor = avgHeuristicPenalty/avgRandomPenalty;

		for (auto o : observers) o->notifyRunEnded();

		delete graph;
		delete graphBuilder;
	}
	for (auto o : observers) o->notifyBenchmarkEnded();

	delete terminalObserver;

	return 0;
}
