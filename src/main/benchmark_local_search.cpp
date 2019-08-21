#include "heuristic.h"
#include "benchmark.h"
#include <cstring>

#define DEFAULT_NUMBER_OF_VERTICES 500
#define DEFAULT_NUMBER_OF_RUNS 100
#define DEFAULT_MAX_VERTEX_DEGREE 10
#define DEFAULT_MIN_VERTEX_DEGREE 4
#define DEFAULT_CYCLE 24
#define DEFAULT_GRAPH_MODEL GraphModel::ADJACENCY_LIST
#define DEFAULT_STOP_CRITERIA stop_criteria::numberOfIterations(10000)

#define WRONG_ARGUMENTS_EXIT_CODE 1

using namespace std;
using namespace traffic;
using namespace benchmark;

enum GraphModel {
	ADJACENCY_LIST,
	ADJACENCY_MATRIX
};

void setupExecutionParameters (int argc, char** argv, size_t &numberOfVertices, size_t &minVertexDegree, size_t &maxVertexDegree, unsigned &numberOfRuns, TimeUnit &cycle, GraphModel &graphModel, function<bool(const HeuristicMetrics&)>& stopCriteriaNotMet) {
	numberOfVertices = DEFAULT_NUMBER_OF_VERTICES;
	minVertexDegree = DEFAULT_MIN_VERTEX_DEGREE;
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

			} else if (strcmp(argv[i], "--minVertexDegree") == 0) {

				i++;
				if (i >= argc) {
					cout << "--minVertexDegree argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				minVertexDegree = atoi(argv[i]);
				if (maxVertexDegree == 0) {
					cout << "--minVertexDegree argument requires a number greater than 0" << endl;
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

			} else if (strcmp(argv[i], "--cycle") == 0) {

				i++;
				if (i >= argc) {
					cout << "--cycle argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				cycle = atoi(argv[i]);
				if (cycle == 0) {
					cout << "--cycle argument requires a number greater than 0" << endl;
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

	GraphBuilder *graphBuilder = nullptr;
	Graph *graph = nullptr;
	Solution constructedSolution, searchedSolution;
	size_t numberOfVertices, maxVertexDegree, minVertexDegree;
	unsigned numberOfRuns;
	TimeUnit cycle;
	GraphModel graphModel;
	function<bool(const HeuristicMetrics&)> stopCriteriaNotMet;
	TerminalObserver *terminalObserver;
	list<Observer*> observers;
	double avgInitialConstructionPenalty, avgLocalSearchPenalty, avgLowerBound;
	double penaltyFactor, lowerBoundFactor;
	chrono::high_resolution_clock::time_point beginSearch;
	chrono::high_resolution_clock::duration avgSearchDuration;
	string formatedAvgSearchDuration;

	setupExecutionParameters(argc, argv, numberOfVertices, minVertexDegree, maxVertexDegree, numberOfRuns, cycle, graphModel, stopCriteriaNotMet);

	terminalObserver = new TerminalObserver();
	terminalObserver->observeVariable("graph lower bound", avgLowerBound);
	terminalObserver->observeVariable("initial solution penalty", avgInitialConstructionPenalty);
	terminalObserver->observeVariable("local search penalty", avgLocalSearchPenalty);
	terminalObserver->observeVariable("search/initial penalty factor", penaltyFactor);
	terminalObserver->observeVariable("search/lower bound factor", lowerBoundFactor);
	terminalObserver->observeVariable("search duration", formatedAvgSearchDuration);
	observers.push_back(terminalObserver);

	avgLowerBound = 0;
	avgInitialConstructionPenalty = 0;
	avgLocalSearchPenalty = 0;
	avgSearchDuration = chrono::high_resolution_clock::duration(0);
	for (auto o : observers) o->notifyBenchmarkBegun("local search heuristic", numberOfRuns);
	for (unsigned i = 0; i < numberOfRuns; i++) {
		graphBuilder = new GraphBuilder(numberOfVertices, minVertexDegree, maxVertexDegree, 1, cycle-1);
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

		avgSearchDuration = (avgSearchDuration*i + chrono::high_resolution_clock::now() - beginSearch)/(i+1);
		avgInitialConstructionPenalty = (avgInitialConstructionPenalty*i + graph->totalPenalty(constructedSolution))/(i+1);
		avgLocalSearchPenalty = (avgLocalSearchPenalty*i + graph->totalPenalty(searchedSolution))/(i+1);
		avgLowerBound = (avgLowerBound*i + graph->lowerBound())/(i+1);

		penaltyFactor = avgLocalSearchPenalty/avgInitialConstructionPenalty;
		lowerBoundFactor = avgLocalSearchPenalty/avgLowerBound;
		formatedAvgSearchDuration = format_chrono_duration(avgSearchDuration);

		for (auto o : observers) o->notifyRunEnded();

		delete graphBuilder;
		delete graph;
	}

	for (auto o : observers) o->notifyBenchmarkEnded();

	delete terminalObserver;

	return 0;
}
