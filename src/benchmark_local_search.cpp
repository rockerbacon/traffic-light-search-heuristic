#include "heuristic.h"
#include "benchmark.h"
#include <cstring>

#define DEFAULT_NUMBER_OF_VERTICES 500
#define DEFAULT_NUMBER_OF_RUNS 10
#define DEFAULT_MAX_VERTEX_DEGREE 10
#define DEFAULT_CYCLE 20
#define DEFAULT_GRAPH_MODEL GraphModel::ADJACENCY_LIST
#define DEFAULT_NUMBER_OF_PERTURBATIONS 1
#define DEFAULT_STOP_CRITERIA stop_criteria::numberOfIterations(100000)

#define WRONG_ARGUMENTS_EXIT_CODE 1

using namespace std;
using namespace traffic;
using namespace benchmark;

enum GraphModel {
	ADJACENCY_LIST,
	ADJACENCY_MATRIX
};

void setupExecutionParameters (int argc, char** argv, size_t &numberOfVertices, size_t &maxVertexDegree, unsigned &numberOfRuns, TimeUnit &cycle, GraphModel &graphModel, function<bool(const LocalSearchMetrics&)>& stopCriteriaNotMet) {
	numberOfVertices = DEFAULT_NUMBER_OF_VERTICES;
	maxVertexDegree = DEFAULT_MAX_VERTEX_DEGREE;
	numberOfRuns = DEFAULT_NUMBER_OF_RUNS;
	cycle = DEFAULT_CYCLE;
	graphModel = DEFAULT_GRAPH_MODEL;
	stopCriteriaNotMet = DEFAULT_STOP_CRITERIA;

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

			} else if (strcmp(argv[i], "--maxVertexDegree") == 0) {

				i++;
				if (i >= argc) {
					cout << "--maxVertexDegree argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				maxVertexDegree = atoi(argv[i]);
				if (maxVertexDegree == 0) {
					cout << "--maxVertexDegree argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

			} else if (strcmp(argv[i], "--useAdjacencyList") == 0) {

				graphModel = GraphModel::ADJACENCY_LIST;

			} else if (strcmp(argv[i], "--useAdjacencyMatrix") == 0) {

				graphModel = GraphModel::ADJACENCY_MATRIX;

			} else if (strcmp(argv[i], "--iterationsWithoutImprovement") == 0) {

				unsigned numberOfIterations;

				i++;
				if (i >= argc) {
					cout << "--iterationsWithoutImprovement argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				numberOfIterations = atoi(argv[i]);
				if (numberOfIterations == 0) {
					cout << "--iterationsWithoutImprovement argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				stopCriteriaNotMet = stop_criteria::numberOfIterationsWithoutImprovement(numberOfIterations);

			} else if (strcmp(argv[i], "--iterations") == 0) {

				unsigned numberOfIterations;

				i++;
				if (i >= argc) {
					cout << "--iterations argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				numberOfIterations = atoi(argv[i]);
				if (numberOfIterations == 0) {
					cout << "--iterations argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				stopCriteriaNotMet = stop_criteria::numberOfIterations(numberOfIterations);
			} else {
				cout << "unknown argument " << argv[i] << endl;
				exit(WRONG_ARGUMENTS_EXIT_CODE);
			}

			i++;
		}
	}
}

int main (int argc, char** argv) {

	GraphBuilder *graphBuilder = nullptr;
	Graph *graph = nullptr;
	Solution constructedSolution, searchedSolution;
	size_t numberOfVertices, maxVertexDegree;
	unsigned numberOfRuns;
	TimeUnit cycle;
	GraphModel graphModel;
	function<bool(const LocalSearchMetrics&)> stopCriteriaNotMet;
	TerminalObserver *terminalObserver;
	list<Observer*> observers;
	double avgInitialConstructionPenalty, avgLocalSearchPenalty, avgLowerBound;
	double penaltyFactor, lowerBoundFactor;
	chrono::high_resolution_clock::time_point beginSearch;
	chrono::high_resolution_clock::duration avgSearchDuration;
	string formatedAvgSearchDuration;

	setupExecutionParameters(argc, argv, numberOfVertices, maxVertexDegree, numberOfRuns, cycle, graphModel, stopCriteriaNotMet);

	terminalObserver = new TerminalObserver("local search heuristic", numberOfRuns);
	terminalObserver->observeVariable("graph lower bound", avgLowerBound);
	terminalObserver->observeVariable("initial solution penalty", avgInitialConstructionPenalty);
	terminalObserver->observeVariable("local search penalty", avgLocalSearchPenalty);
	terminalObserver->observeVariable("search/initial penalty factor", penaltyFactor);
	terminalObserver->observeVariable("search/lower bound factor", lowerBoundFactor);
	terminalObserver->observeVariable("search duration", formatedAvgSearchDuration);
	observers.push_back(terminalObserver);

	avgInitialConstructionPenalty = 0;
	avgLocalSearchPenalty = 0;
	for (auto o : observers) o->notifyBenchmarkBegun();
	for (unsigned i = 0; i < numberOfRuns; i++) {
		graphBuilder = new GraphBuilder(numberOfVertices, 1, maxVertexDegree, 1, cycle-1);
		graphBuilder->withCycle(cycle);
		switch (graphModel) {
			case GraphModel::ADJACENCY_LIST:
				graph = graphBuilder->buildAsAdjacencyList();
				break;
			case GraphModel::ADJACENCY_MATRIX:
				graph = graphBuilder->buildAsAdjacencyMatrix();
				break;
		}

		for (auto o : observers) o->notifyRunBegun();

		beginSearch = chrono::high_resolution_clock::now();
		constructedSolution = constructHeuristicSolution(*graph);
		searchedSolution = localSearchHeuristic(*graph, constructedSolution, stopCriteriaNotMet);

		if (i == 0) {
			avgSearchDuration = chrono::high_resolution_clock::now() - beginSearch;
			avgInitialConstructionPenalty = graph->totalPenalty(constructedSolution);
			avgLocalSearchPenalty = graph->totalPenalty(searchedSolution);
			avgLowerBound = graph->lowerBound();
		} else {
			avgSearchDuration = (avgSearchDuration + chrono::high_resolution_clock::now() - beginSearch)/2;
			avgInitialConstructionPenalty = (avgInitialConstructionPenalty + graph->totalPenalty(constructedSolution))/2;
			avgLocalSearchPenalty = (avgLocalSearchPenalty + graph->totalPenalty(searchedSolution))/2;
			avgLowerBound = (avgLowerBound+graph->lowerBound())/2;
		}
		penaltyFactor = avgLocalSearchPenalty/avgInitialConstructionPenalty;
		lowerBoundFactor = avgLocalSearchPenalty/avgLowerBound;
		formatedAvgSearchDuration = format_chrono_duration(avgSearchDuration);

		for (auto o : observers) {
			o->notifyRunUpdate();
			o->notifyRunEnded();
		}

		delete graphBuilder;
		delete graph;
	}

	for (auto o : observers) o->notifyBenchmarkEnded();

	delete terminalObserver;

	return 0;
}
