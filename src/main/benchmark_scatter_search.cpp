#include "heuristic/heuristic.h"
#include "assertions/benchmark.h"
#include "assertions/command_line_interface.h"

#define DEFAULT_NUMBER_OF_RUNS 3 
#define DEFAULT_STOP_FUNCTION stop_function_factory::numberOfIterations(500)
#define DEFAULT_ELITE_POPULATION_SIZE 1
#define DEFAULT_DIVERSE_POPULATION_SIZE 9
#define DEFAULT_LOCAL_SEARCH_ITERATIONS 50
#define DEFAULT_NUMBER_OF_THREADS 1

using namespace std;
using namespace traffic;
using namespace benchmark;
using namespace heuristic;

int main (int argc, char** argv) {

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

	/* CLI INTERFACE */
	cli::RequiredArgument<string> inputPath("input", 'i');

	cli::OptionalArgument<unsigned> numberOfRuns("runs", DEFAULT_NUMBER_OF_RUNS);

	cli::FlagArgument useAdjacencyMatrix("useAdjacencyMatrix");

	cli::OptionalArgument<unsigned> numberOfIterationsToStop("iterations", 0);
	cli::OptionalArgument<unsigned> numberOfIterationsWithoutImprovementToStop("iterationsWithoutImprovement", 0);

	cli::OptionalArgument<double> crossoverMutationProbability("useCrossoverWithMutationProbability", -1.0);

	cli::OptionalArgument<size_t> elitePopulationSize("elitePopulationSize", DEFAULT_ELITE_POPULATION_SIZE);
	cli::OptionalArgument<size_t> diversePopulationSize("diversePopulationSize", DEFAULT_DIVERSE_POPULATION_SIZE);
	cli::OptionalArgument<unsigned> localSearchIterations("localSearchIterations", DEFAULT_LOCAL_SEARCH_ITERATIONS);

	cli::OptionalArgument<unsigned> numberOfThreads("threads", DEFAULT_NUMBER_OF_THREADS, 't');

	cli::capture_all_arguments_from(argc, argv);

	graphFile.open(*inputPath);
	graphBuilder.read_from_file(graphFile);

	if (*useAdjacencyMatrix) {
		graph = graphBuilder.buildAsAdjacencyMatrix();
	} else {
		graph = graphBuilder.buildAsAdjacencyList();
	}

	if (numberOfIterationsToStop.is_present()) {
		stopFunction = stop_function_factory::numberOfIterations(*numberOfIterationsToStop);
	} else if (numberOfIterationsWithoutImprovementToStop.is_present()) {
		stopFunction = stop_function_factory::numberOfIterationsWithoutImprovement(*numberOfIterationsWithoutImprovementToStop);
	} else {
		stopFunction = DEFAULT_STOP_FUNCTION;
	}	

	if (crossoverMutationProbability.is_present()) {
		combinationMethod = combination_method_factory::crossover(*crossoverMutationProbability);
	} else {
		combinationMethod = combination_method_factory::breadthFirstSearch();	
	}
	/* CLI INTERFACE */

	register_observer(new TerminalObserver());

	observe_variable("graph lower bound", lowerBound, observation_mode::AVERAGE_VALUE);
	observe_variable("populational heuristics penalty", penalty, observation_mode::AVERAGE_VALUE);
	observe_variable("populational heuristic/lower bound factor", lowerBoundFactor, observation_mode::AVERAGE_VALUE);
	observe_variable("populational heuristic duration", duration, observation_mode::AVERAGE_VALUE);

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
}
