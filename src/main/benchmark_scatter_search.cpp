#include "heuristic/heuristic.h"
#include <stopwatch/stopwatch.h>
#include <cpp-benchmark/benchmark.h>
#include <cpp-command-line-interface/command_line_interface.h>
#include <fstream>
#include <thread>

#define DEFAULT_NUMBER_OF_RUNS 10
#define DEFAULT_STOP_FUNCTION stop_function_factory::numberOfIterations(80)
#define DEFAULT_ELITE_POPULATION_SIZE 64
#define DEFAULT_DIVERSE_POPULATION_SIZE 1280
#define DEFAULT_LOCAL_SEARCH_ITERATIONS 20000
#define DEFAULT_NUMBER_OF_THREADS std::thread::hardware_concurrency()
#define DEFAULT_MUTATION_PROBABILITY 0.595

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
	cli::OptionalArgument<unsigned> minutesToStop(0, "minutes", "minutes after which to stop the heuristic");
	cli::OptionalArgument<unsigned> secondsToStop(0, "seconds", "seconds after which to stop the heuristic");
	cli::OptionalArgument<TimeUnit> penaltyToStop(0, "penalty", "target penalty where heuristic should stop");

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
	} else if (secondsToStop.is_present()) {
		stopFunction = stop_function_factory::executionTime(chrono::seconds(*secondsToStop));
	} else if (penaltyToStop.is_present()) {
		stopFunction = stop_function_factory::penalty(*penaltyToStop);
	} else {
		stopFunction = DEFAULT_STOP_FUNCTION;
	}

	if (*useCrossover) {
		combinationMethod = combination_method_factory::crossover(*mutationProbability);
	} else {
		combinationMethod = combination_method_factory::breadthFirstSearch(*mutationProbability);
	}

	TerminalObserver terminalObserver;

	register_observers(terminalObserver);
	// if (*outputPath != DONT_OUTPUT_TO_FILE) {
	//   register_observer(new TsvFileObserver(*outputPath));
	// }

	observe(lowerBound, lower_bound);
	observe(penalty, current_penalty);
	observe_average(penalty, avg_penalty);
	observe_minimum(penalty, min_penalty);
	observe_maximum(penalty, max_penalty);

	observe(duration, current_duration);
	observe_average(duration, avg_duration);
	observe_minimum(duration, min_duration);
	observe_maximum(duration, max_duration);

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
	};

	delete graph;

	return 0;
} end_cli_main;
