#include <iostream>
#include <string>
#include <cstring>

#include "heuristic.h"
#include "benchmark.h"

#define DEFAULT_NUMBER_OF_VERTICES 10
#define DEFAULT_NUMBER_OF_RUNS 10
#define DEFAULT_CYCLE 20

#define WRONG_ARGUMENTS_EXIT_CODE 1

using namespace std;
using namespace traffic;
using namespace benchmark;

void setupExecutionParameters (int argc, char** argv, size_t &numberOfVertices, unsigned &numberOfRuns, TimeUnit &cycle) {
	numberOfVertices = DEFAULT_NUMBER_OF_VERTICES;
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
	size_t numberOfVertices;
	unsigned numberOfRuns;
	TimeUnit totalRandomVariety, totalHeuristicVariety;
	TimeUnit totalRandomPenalty, totalHeuristicPenalty;
	TimeUnit cycle;
	Solution *randomSolution, *heuristicSolution;
	list<Solution*> randomSolutions, heuristicSolutions;
	double varietyFactor, penaltyFactor;

	setupExecutionParameters(argc, argv, numberOfVertices, numberOfRuns, cycle);

	terminalObserver = new TerminalObserver("initial solution construction", numberOfRuns);
	terminalObserver->observeVariable("Random construction variety", totalRandomVariety);
	terminalObserver->observeVariable("Random construction penalty", totalRandomPenalty);
	terminalObserver->observeVariable("Heuristic construction variety", totalHeuristicVariety);
	terminalObserver->observeVariable("Heuristic construction penalty", totalHeuristicPenalty);
	terminalObserver->observeVariable("Heuristic/Random variety factor", varietyFactor);
	terminalObserver->observeVariable("Heuristic/Random penalty factor", penaltyFactor);
	observers.push_back(terminalObserver);

	totalRandomVariety = 0;
	totalHeuristicVariety = 0;
	totalRandomPenalty = 0;
	totalHeuristicPenalty = 0;
	for (auto o : observers) o->notifyBenchmarkBegun();
	for (unsigned i = 0; i < numberOfRuns; i++) {
		graphBuilder = new GraphBuilder(numberOfVertices, 1, numberOfVertices/3, 1, cycle-1);
		graphBuilder->withCycle(cycle);
		graph = graphBuilder->buildAsAdjacencyList();

		for (auto o : observers) o->notifyRunBegun();

		constructRandomSolution(*graph);
		totalRandomPenalty += graph->totalPenalty();
		randomSolution = graph->extractSolution();
		for (auto previousSolution : randomSolutions) {
			totalRandomVariety += distance(*graph, *previousSolution, *randomSolution);
		}
		randomSolutions.push_back(randomSolution);

		constructHeuristicSolution(*graph);
		totalHeuristicPenalty += graph->totalPenalty();
		heuristicSolution = graph->extractSolution();
		for (auto previousSolution : heuristicSolutions) {
			totalHeuristicVariety += distance(*graph, *previousSolution, *heuristicSolution);
		}
		heuristicSolutions.push_back(heuristicSolution);

		varietyFactor = (double)totalHeuristicVariety/totalRandomVariety;
		penaltyFactor = (double)totalHeuristicPenalty/totalRandomPenalty;

		for (auto o : observers) {
			o->notifyRunUpdate();
			o->notifyRunEnded();
		}

		delete graph;
		delete graphBuilder;
	}
	for (auto o : observers) o->notifyBenchmarkEnded();

	delete terminalObserver;
	for (auto solution : randomSolutions) {
		delete solution;
	}
	for (auto solution : heuristicSolutions) {
		delete solution;
	}

	return 0;
}
