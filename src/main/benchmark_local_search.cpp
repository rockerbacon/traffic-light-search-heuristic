#include "heuristic/heuristic.h"
#include "assertions/benchmark.h"
#include "assertions/command_line_interface.h"
#include <cstring>

#define DEFAULT_NUMBER_OF_RUNS 100
#define DEFAULT_GRAPH_MODEL GraphModel::ADJACENCY_LIST
#define DEFAULT_STOP_FUNCTION stop_function_factory::numberOfIterations(10000)

using namespace std;
using namespace traffic;
using namespace benchmark;
using namespace heuristic;

int main (int argc, char** argv) {

	GraphBuilder graphBuilder;
	Graph *graph = nullptr;
	Solution constructedSolution, searchedSolution;
	size_t numberOfVertices, maxVertexDegree, minVertexDegree;
	TimeUnit cycle;
	StopFunction stopFunction;
	double initialConstructionPenalty, localSearchPenalty, lowerBound;
	double penaltyFactor, lowerBoundFactor;
	chrono::high_resolution_clock::time_point beginSearch;
	chrono::high_resolution_clock::duration searchDuration;
	ifstream fileInputStream;

	/* CLI ARGUMENTS */
	cli::RequiredArgument<string> inputFilePath("input", 'i');

	cli::OptionalArgument<unsigned> numberOfRuns("runs", DEFAULT_NUMBER_OF_RUNS);

	cli::FlagArgument useAdjacencyMatrix("useAdjacencyMatrix");
	cli::FlagArgument useAdjacencyList("useAdjacencyList");

	cli::OptionalArgument<unsigned> stopAfterNumberOfIterations("iterations", 0);
	cli::OptionalArgument<unsigned> stopAfterIterationsWithoutImprovement("iterationsWithoutImprovement", 0);
	cli::OptionalArgument<unsigned> stopAfterSeconds("seconds", 0);
	cli::OptionalArgument<unsigned> stopAfterMinutes("minutes", 0);

	fileInputStream.open(*inputFilePath);
	graphBuilder.read_from_file(fileInputStream);

	if (*useAdjacencyMatrix) {
		graph = graphBuilder.buildAsAdjacencyMatrix();
	} else {
		graph = graphBuilder.buildAsAdjacencyList();
	}

	if (stopAfterNumberOfIterations.is_present()) {
		stopFunction = stop_function_factory::numberOfIterations(*stopAfterNumberOfIterations);
	} else if (stopAfterIterationsWithoutImprovement.is_present()) {
		stopFunction = stop_function_factory::numberOfIterationsWithoutImprovement(*stopAfterIterationsWithoutImprovement);
	} else if (stopAfterSeconds.ArgumentInterface::is_present()) {
		stopFunction = stop_function_factory::executionTime(chrono::seconds(*stopAfterSeconds));
	} else if (stopAfterMinutes.is_present()) {
		stopFunction = stop_function_factory::executionTime(chrono::minutes(*stopAfterMinutes));
	} else {
		stopFunction = DEFAULT_STOP_FUNCTION;
	}

	cli::capture_all_arguments_from(argc, argv);
	/* CLI ARGUMENTS */

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
}
