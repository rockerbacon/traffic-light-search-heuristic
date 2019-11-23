#include "heuristic/heuristic.h"
#include <cpp-benchmark/benchmark.h>
#include <cpp-command-line-interface/command_line_interface.h>
#include <cstring>

#define DEFAULT_NUMBER_OF_RUNS 100
#define DEFAULT_GRAPH_MODEL GraphModel::ADJACENCY_LIST
#define DEFAULT_STOP_FUNCTION stop_function_factory::numberOfIterations(10000)

using namespace std;
using namespace traffic;
using namespace benchmark;
using namespace heuristic;

cli_main (
		"Benchmark Local Search",
		"unknown",
		"Program for benchmarking local search heuristic",

		cli::RequiredArgument<string> inputFilePath("input", "path to file containing the problem instance");
		cli::OptionalArgument<unsigned> numberOfRuns(DEFAULT_NUMBER_OF_RUNS, "runs", "number of runs for the benchmark");
		cli::OptionalArgument<unsigned> numberOfIterationsToStop(0, "iterations", "number of iterations to execute the search");
		cli::OptionalArgument<unsigned> numberOfIterationsWithoutImprovementToStop(0, "iterationsWithoutImprovement", "number of iterations without improvement after which heuristic should stop");
		cli::OptionalArgument<unsigned> minutesToStop(0, "minutes", "minutes after which local search should stop");

		cli::FlagArgument useAdjacencyMatrix("useAdjacencyMatrix", "use adjacency matrix instead of adjacency list");
) {

	GraphBuilder graphBuilder;
	Graph *graph = nullptr;
	Solution constructedSolution, searchedSolution;
	StopFunction stopFunction;
	double initialConstructionPenalty, localSearchPenalty, lowerBound;
	double penaltyFactor, lowerBoundFactor;
	chrono::high_resolution_clock::time_point beginSearch;
	chrono::high_resolution_clock::duration searchDuration;
	ifstream fileInputStream;

	fileInputStream.open(*inputFilePath);
	graphBuilder.read_from_file(fileInputStream);
	fileInputStream.close();

	if (*useAdjacencyMatrix) {
		graph = graphBuilder.buildAsAdjacencyMatrix();
	} else {
		graph = graphBuilder.buildAsAdjacencyList();
	}

	if (numberOfIterationsToStop.is_present()) {
		stopFunction = stop_function_factory::numberOfIterations(*numberOfIterationsToStop);
	} else if (numberOfIterationsWithoutImprovementToStop.is_present()) {
		stopFunction = stop_function_factory::numberOfIterationsWithoutImprovement(*numberOfIterationsWithoutImprovementToStop);
	} else if (minutesToStop.is_present()) {
		stopFunction = stop_function_factory::executionTime(chrono::minutes(*minutesToStop));
	} else {
		stopFunction = DEFAULT_STOP_FUNCTION;
	}

	register_observer(new TerminalObserver());

	observe_variable("graph lower bound", lowerBound, observation_mode::AVERAGE_VALUE);
	observe_variable("initial solution penalty", initialConstructionPenalty, observation_mode::AVERAGE_VALUE);
	observe_variable("local search penalty", localSearchPenalty, observation_mode::AVERAGE_VALUE);
	observe_variable("search/initial penalty factor", penaltyFactor, observation_mode::AVERAGE_VALUE);
	observe_variable("search/lower bound factor", lowerBoundFactor, observation_mode::AVERAGE_VALUE);
	observe_variable("search duration", searchDuration, observation_mode::AVERAGE_VALUE);

	benchmark("local search heuristic", *numberOfRuns) {

		beginSearch = chrono::high_resolution_clock::now();
		constructedSolution = constructHeuristicSolution(*graph);
		searchedSolution = localSearchHeuristic(*graph, constructedSolution, stopFunction);

		searchDuration = chrono::high_resolution_clock::now() - beginSearch;
		initialConstructionPenalty = graph->totalPenalty(constructedSolution);
		localSearchPenalty = graph->totalPenalty(searchedSolution);
		lowerBound = graph->lowerBound();

		penaltyFactor = localSearchPenalty/initialConstructionPenalty;
		lowerBoundFactor = localSearchPenalty/lowerBound;

	} end_benchmark;

	delete graph;
	delete_observers();

	return 0;
} end_cli_main;
