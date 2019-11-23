#include "heuristic/heuristic.h"
#include <cpp-benchmark/benchmark.h>
#include <cpp-command-line-interface/command_line_interface.h>
#include <fstream>

#define DEFAULT_NUMBER_OF_RUNS 10
#define DEFAULT_STOP_FUNCTION stop_function_factory::numberOfIterations(500)
#define DEFAULT_ELITE_POPULATION_SIZE 10
#define DEFAULT_DIVERSE_POPULATION_SIZE 100
#define DEFAULT_LOCAL_SEARCH_ITERATIONS 50
#define DEFAULT_NUMBER_OF_THREADS 1
#define DEFAULT_MUTATION_PROBABILITY 0.1

#define DONT_OUTPUT_TO_FILE ""

using namespace std;
using namespace traffic;
using namespace benchmark;
using namespace heuristic;

cli_main (
	"benchmark_scatter_search",
	"undefined",
	"Benchmark Scatter Search Heuristic for the simplified traffic light problem",

	cli::RequiredArgument<string> inputPath("input", "path to file containing the problem instance");
	cli::OptionalArgument<string> outputPath(DONT_OUTPUT_TO_FILE, "output", "path to where benchmark results will be output (tsv format)");

	cli::OptionalArgument<unsigned> numberOfRuns(DEFAULT_NUMBER_OF_RUNS, "runs", "number of times to execute benchmark");

	cli::FlagArgument useAdjacencyMatrix("useAdjacencyMatrix", "if present will execute scatter search using an adjacency matrix instead of an adjacency list");

	cli::OptionalArgument<unsigned> numberOfIterationsToStop(0, "iterations", "stop heuristic after specified number of iterations");
	cli::OptionalArgument<unsigned> minutesToStop(0, "minutes", "stop heuristic after specified time has passed");

	cli::OptionalArgument<double> mutationProbability(DEFAULT_MUTATION_PROBABILITY, "mutationProbability", "mutation probability to use during combination");
	cli::FlagArgument useCrossover("useCrossover", "use crossover as combination method. Default combination is a Breadth-First search combination");

	cli::OptionalArgument<size_t> elitePopulationSize(DEFAULT_ELITE_POPULATION_SIZE, "elite", "specify size for the elite population");
	cli::OptionalArgument<size_t> diversePopulationSize(DEFAULT_DIVERSE_POPULATION_SIZE, "diverse", "specify size for the diverse population");
	cli::OptionalArgument<unsigned> localSearchIterations(DEFAULT_LOCAL_SEARCH_ITERATIONS, "localSearchIterations", "specify number of iterations the improvement method should execute");

	cli::OptionalArgument<unsigned> numberOfThreads(DEFAULT_NUMBER_OF_THREADS, "threads", "specify number of threads");
) {

	GraphBuilder graphBuilder;
	Graph *graph = nullptr;
	Solution solution;
	StopFunction stopFunction;
	CombinationMethod combinationMethod;
	double penalty, lowerBound;
	double lowerBoundFactor;
	chrono::high_resolution_clock::time_point begin;
	chrono::high_resolution_clock::duration duration;
	ifstream graphFile;

	graphFile.open(*inputPath);
	graphBuilder.read_from_file(graphFile);

	if (*useAdjacencyMatrix) {
		graph = graphBuilder.buildAsAdjacencyMatrix();
	} else {
		graph = graphBuilder.buildAsAdjacencyList();
	}

	if (numberOfIterationsToStop.is_present()) {
		stopFunction = stop_function_factory::numberOfIterations(*numberOfIterationsToStop);
	} else if (minutesToStop.is_present()) {
		stopFunction = stop_function_factory::executionTime(chrono::minutes(*minutesToStop));
	} else {
		stopFunction = DEFAULT_STOP_FUNCTION;
	}

	if (*useCrossover) {
		combinationMethod = combination_method_factory::crossover(*mutationProbability);
	} else {
		combinationMethod = combination_method_factory::breadthFirstSearch(*mutationProbability);
	}

	register_observer(new TerminalObserver());
	if (*outputPath != DONT_OUTPUT_TO_FILE) {
		register_observer(new TsvFileObserver(*outputPath));
	}

	observe_variable("graph lower bound", lowerBound, observation_mode::CURRENT_VALUE);
	observe_variable("populational heuristics penalty", penalty, observation_mode::AVERAGE_VALUE | observation_mode::MAXIMUM_VALUE | observation_mode::MINIMUM_VALUE);
	observe_variable("populational heuristic/lower bound factor", lowerBoundFactor, observation_mode::AVERAGE_VALUE | observation_mode::MAXIMUM_VALUE | observation_mode::MINIMUM_VALUE);
	observe_variable("populational heuristic duration", duration, observation_mode::AVERAGE_VALUE | observation_mode::MAXIMUM_VALUE);

	benchmark("scatter search heuristic", *numberOfRuns) {

		begin = chrono::high_resolution_clock::now();

		if (*numberOfThreads < 2) {
			solution = scatterSearch(*graph, *elitePopulationSize, *diversePopulationSize, *localSearchIterations, stopFunction, combinationMethod);
		} else {
			solution = parallel::scatterSearch(*graph, *elitePopulationSize, *diversePopulationSize, *localSearchIterations, stopFunction, combinationMethod, *numberOfThreads);
		}

		duration = chrono::high_resolution_clock::now() - begin;
		penalty = graph->totalPenalty(solution);
		lowerBound = graph->lowerBound();

		lowerBoundFactor = penalty/lowerBound;

	} end_benchmark;

	delete graph;
	delete_observers();

	return 0;
} end_cli_main;
