#include "heuristic/heuristic.h"
#include "assertions/benchmark.h"
#include "assertions/command_line_interface.h"

#define DEFAULT_NUMBER_OF_RUNS 10
#define DEFAULT_STOP_FUNCTION stop_function_factory::numberOfIterations(330)
#define DEFAULT_POPULATION_SIZE 14
#define DEFAULT_MUTATION_PROBABILITY 0.003

using namespace std;
using namespace traffic;
using namespace benchmark;
using namespace heuristic;

int main (int argc, char** argv) {

	GraphBuilder graphBuilder;
	Graph *graph = nullptr;
	Solution solution;
	CombinationMethod combinationMethod;
	StopFunction stopFunction;

	double penalty, lowerBound;
	double penaltyFactor, lowerBoundFactor;
	chrono::high_resolution_clock::time_point begin;
	chrono::high_resolution_clock::duration duration;
	ifstream fileInputStream;

	/* CLI ARGUMENTS */
	cli::RequiredArgument<string> inputFilePath("input", 'i');

	cli::OptionalArgument<unsigned> numberOfRuns("runs", DEFAULT_NUMBER_OF_RUNS);

	cli::FlagArgument useAdjacencyMatrix("useAdjacencyMatrix");

	cli::OptionalArgument<size_t> populationSize("populationSize", DEFAULT_POPULATION_SIZE);
	cli::OptionalArgument<double> mutationProbability("mutationProbability", DEFAULT_MUTATION_PROBABILITY);
	cli::FlagArgument useCrossover("useCrossover");
	cli::FlagArgument useBreadthFirstSearch("useBfs");

	cli::OptionalArgument<unsigned> numberOfIterationsToStop("iterations", 0);
	cli::OptionalArgument<unsigned> numberOfIterationsWithoutImprovementToStop("numberOfIterationsWithoutImprovement", 0);
	cli::OptionalArgument<unsigned> secondsToStop("seconds", 0);
	cli::OptionalArgument<unsigned> minutesToStop("minutes", 0);

	cli::capture_all_arguments_from(argc, argv);

	fileInputStream.open(*inputFilePath);
	graphBuilder.read_from_file(fileInputStream);

	if (*useAdjacencyMatrix) {
		graph = graphBuilder.buildAsAdjacencyMatrix();
	} else {
		graph = graphBuilder.buildAsAdjacencyList();
	}

	if (*useBreadthFirstSearch) {
		combinationMethod = combination_method_factory::breadthFirstSearch();
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

	register_observer(new TerminalObserver());

	observe_variable("graph lower bound", lowerBound, observation_mode::AVERAGE_VALUE);
	observe_variable("genetic algorithm penalty", penalty, observation_mode::AVERAGE_VALUE);
	observe_variable("genetic algorithm/lower bound factor", lowerBoundFactor, observation_mode::AVERAGE_VALUE);
	observe_variable("genetic algorithm duration", duration, observation_mode::AVERAGE_VALUE);

	lowerBound = graph->lowerBound();
	benchmark("genetic algorithm", *numberOfRuns) {

		begin = chrono::high_resolution_clock::now();
		solution = geneticAlgorithm(*graph, *populationSize, stopFunction, combinationMethod);

		duration = chrono::high_resolution_clock::now() - begin;
		penalty = graph->totalPenalty(solution);
		lowerBound = graph->lowerBound();

		lowerBoundFactor = penalty/lowerBound;

	} end_benchmark;

	delete_observers();
	delete graph;

	return 0;
}
