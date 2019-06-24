#include <iostream>
#include <string>
#include <cstring>

#include "heuristic.h"
#include "benchmark.h"

#define DEFAULT_NUMBER_OF_VERTICES 500
#define DEFAULT_NUMBER_OF_RUNS 1000
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

	setupExecutionParameters(argc, argv, numberOfVertices, minVertexDegree, maxVertexDegree, numberOfRuns, cycle);

	terminalObserver = new TerminalObserver("initial solution construction", numberOfRuns);
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
	observers.push_back(terminalObserver);

	avgRandomVariety = 0;
	avgHeuristicVariety = 0;
	for (auto o : observers) o->notifyBenchmarkBegun();
	for (unsigned i = 0; i < numberOfRuns; i++) {
		graphBuilder = new GraphBuilder(numberOfVertices, minVertexDegree, maxVertexDegree, 1, cycle-1);
		graphBuilder->withCycle(cycle);
		graph = graphBuilder->buildAsAdjacencyList();

		for (auto o : observers) o->notifyRunBegun();

		beginTime = chrono::high_resolution_clock::now();
		solution = constructRandomSolution(*graph);
		if (i == 0) {
			avgRandomTime = chrono::high_resolution_clock::now() - beginTime;
			avgRandomPenalty = graph->totalPenalty(solution);
			avgLowerBound = graph->lowerBound();
		} else {
			avgRandomTime = (avgRandomTime + chrono::high_resolution_clock::now() - beginTime)/2;
			avgRandomPenalty = (avgRandomPenalty+graph->totalPenalty(solution))/2;
			avgLowerBound = (avgLowerBound+graph->lowerBound())/2;

			it = randomSolutions.begin();
			if (avgRandomVariety == 0) {
				avgRandomVariety = distance(*graph, *it, solution);
			} else {
				avgRandomVariety = (avgRandomVariety+distance(*graph, *it, solution))/2;
			}
			for (it++; it != randomSolutions.end(); it++) {
				avgRandomVariety = (avgRandomVariety+distance(*graph, *it, solution))/2;
			}

		}
		formatedAvgRandomTime = format_chrono_duration(avgRandomTime);
		lowerBoundRandomFactor = avgRandomPenalty/avgLowerBound;

		randomSolutions.push_back(solution);

		beginTime = chrono::high_resolution_clock::now();
		solution = constructHeuristicSolution(*graph);
		if (i == 0) {
			avgHeuristicTime = chrono::high_resolution_clock::now() - beginTime;
			avgHeuristicPenalty = graph->totalPenalty(solution);
		} else {
			avgHeuristicTime = (avgHeuristicTime + chrono::high_resolution_clock::now() - beginTime)/2;
			avgHeuristicPenalty = (avgHeuristicPenalty + graph->totalPenalty(solution))/2;

			it = heuristicSolutions.begin();
			if (avgHeuristicVariety == 0) {
				avgHeuristicVariety = distance(*graph, *it, solution);
			} else {
				avgHeuristicVariety = avgHeuristicVariety+distance(*graph, *it, solution);
			}
			for (it++; it != heuristicSolutions.end(); it++) {
				avgHeuristicVariety = (avgHeuristicVariety+distance(*graph, *it, solution))/2;
			}

		}
		formatedAvgHeuristicTime = format_chrono_duration(avgHeuristicTime);
		lowerBoundHeuristicFactor = avgHeuristicPenalty/avgLowerBound;

		heuristicSolutions.push_back(solution);

		varietyFactor = avgHeuristicVariety/avgRandomVariety;
		penaltyFactor = avgHeuristicPenalty/avgRandomPenalty;

		for (auto o : observers) {
			o->notifyRunUpdate();
			o->notifyRunEnded();
		}

		delete graph;
		delete graphBuilder;
	}
	for (auto o : observers) o->notifyBenchmarkEnded();

	delete terminalObserver;

	return 0;
}
