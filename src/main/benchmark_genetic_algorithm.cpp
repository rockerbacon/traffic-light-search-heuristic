#include "heuristic/heuristic.h"
#include <stopwatch/stopwatch.h>
#include <cpp-benchmark/benchmark.h>
#include <cpp-command-line-interface/command_line_interface.h>

#define DEFAULT_NUMBER_OF_RUNS 10
#define DEFAULT_STOP_FUNCTION stop_function_factory::numberOfIterations(330)
#define DEFAULT_POPULATION_SIZE 14
#define DEFAULT_MUTATION_PROBABILITY 0.003

using namespace std;
using namespace traffic;
using namespace benchmark;
using namespace heuristic;

cli_main(
	"Benchmark Genetic Algorithm",
	"v0.1",
	"Benchmarks a genetic algorithm against a randomly generated solution",

	cli::RequiredArgument<string> inputFilePath("input", "input file");
	cli::create_alias("input", 'i');

	cli::OptionalArgument<unsigned> numberOfRuns(DEFAULT_NUMBER_OF_RUNS, "runs");

	cli::FlagArgument useAdjacencyMatrix("useAdjacencyMatrix");

	cli::OptionalArgument<size_t> populationSize(DEFAULT_POPULATION_SIZE, "populationSize");
	cli::OptionalArgument<double> mutationProbability(DEFAULT_MUTATION_PROBABILITY, "mutationProbability");
	cli::FlagArgument useCrossover("useCrossover");
	cli::FlagArgument useBreadthFirstSearch("useBfs");

	cli::OptionalArgument<unsigned> numberOfIterationsToStop(0, "iterations");
	cli::OptionalArgument<unsigned> numberOfIterationsWithoutImprovementToStop(0, "numberOfIterationsWithoutImprovement");
	cli::OptionalArgument<unsigned> secondsToStop(0, "seconds");
	cli::OptionalArgument<unsigned> minutesToStop(0, "minutes");
) {

	GraphBuilder graphBuilder;
	Graph *graph = nullptr;
	Solution solution;
	CombinationMethod combinationMethod;
	StopFunction stopFunction;

	double penalty, lowerBound;
	double lowerBoundFactor;
	chrono::high_resolution_clock::time_point begin;
	chrono::high_resolution_clock::duration duration;
	ifstream fileInputStream;

	/* CLI ARGUMENTS */
	cli::capture_all_arguments_from(argc, argv);

	fileInputStream.open(*inputFilePath);
	graphBuilder.read_from_file(fileInputStream);

	if (*useAdjacencyMatrix) {
		graph = graphBuilder.buildAsAdjacencyMatrix();
	} else {
		graph = graphBuilder.buildAsAdjacencyList();
	}

	if (*useBreadthFirstSearch) {
		combinationMethod = combination_method_factory::breadthFirstSearch(*mutationProbability);
	} else {
		combinationMethod = combination_method_factory::crossover(*mutationProbability);
	}

	if (numberOfIterationsToStop.is_present()) {
		stopFunction = stop_function_factory::numberOfIterations(*numberOfIterationsToStop);
	} else if (numberOfIterationsWithoutImprovementToStop.is_present()) {
		stopFunction = stop_function_factory::numberOfIterationsWithoutImprovement(*numberOfIterationsWithoutImprovementToStop);
	} else if (secondsToStop.is_present()) {
		stopFunction = stop_function_factory::executionTime(chrono::seconds(*secondsToStop));
	} else if (minutesToStop.is_present()) {
		stopFunction = stop_function_factory::executionTime(chrono::minutes(*minutesToStop));
	} else {
		stopFunction = DEFAULT_STOP_FUNCTION;
	}
	/* CLI ARGUMENTS */

	TerminalObserver terminalObserver;
	register_observers(terminalObserver);

	observe_average(lowerBound, graph_lower_bound);
	observe_average(penalty, avg_penalty);
	observe_average(lowerBoundFactor, lower_bound_factor);
	observe_average(duration, avg_duration);

	lowerBound = graph->lowerBound();
	benchmark("genetic algorithm", *numberOfRuns) {

		begin = chrono::high_resolution_clock::now();
		solution = geneticAlgorithm(*graph, *populationSize, stopFunction, combinationMethod);

		duration = chrono::high_resolution_clock::now() - begin;
		penalty = graph->totalPenalty(solution);
		lowerBound = graph->lowerBound();

		lowerBoundFactor = penalty/lowerBound;

	};

	delete graph;

	return 0;
} end_cli_main;
