#include "heuristic/heuristic.h"
#include "assertions/benchmark.h"
#include "assertions/command_line_interface.h"

#define DEFAULT_NUMBER_OF_RUNS 100

using namespace std;
using namespace traffic;
using namespace benchmark;
using namespace heuristic;

double iterativeVariety (const Graph &graph, const list<Solution>& previousSolutions, const Solution &newSolution) {
	double variety;	
	int j;
	list<Solution>::const_iterator it;

	variety = 0;
	for (j = 0, it = previousSolutions.begin(); it != previousSolutions.end(); it++, j++) {
		variety = (variety*j + distance(graph, *it, newSolution))/(j+1);
	}

	return variety;
}

int main (int argc, char** argv) {

	GraphBuilder graphBuilder;
	Graph *graph;
	double randomVariety, heuristicVariety;
	double randomPenalty, heuristicPenalty;
	double lowerBound;
	list<Solution> randomSolutions, heuristicSolutions;
	double varietyFactor, penaltyFactor;
	chrono::high_resolution_clock::duration randomTime, heuristicTime;
	chrono::high_resolution_clock::time_point beginTime;
	double lowerBoundRandomFactor, lowerBoundHeuristicFactor;
	Solution solution;
	ifstream fileInputStream;

	/* CLI ARGUMENTS */
	cli::RequiredArgument<string> inputFilePath("input", 'i');

	cli::OptionalArgument<unsigned> numberOfRuns("runs", DEFAULT_NUMBER_OF_RUNS);

	cli::FlagArgument useAdjacencyList("useAdjacencyList");
	cli::FlagArgument useAdjacencyMatrix("useAdjacencyMatrix");

	cli::capture_all_arguments_from(argc, argv);

	fileInputStream.open(*inputFilePath);
	graphBuilder.read_from_file(fileInputStream);

	if (*useAdjacencyMatrix) {
		graph = graphBuilder.buildAsAdjacencyMatrix();
	} else {
		graph = graphBuilder.buildAsAdjacencyList();
	}
	/* CLI ARGUMENTS */

	register_observer(new TerminalObserver());

	observe_variable("Lower bound", lowerBound, observation_mode::CURRENT_VALUE);
	observe_variable("Random construction variety", randomVariety, observation_mode::AVERAGE_VALUE);
	observe_variable("Random construction penalty", randomPenalty, observation_mode::AVERAGE_VALUE);
	observe_variable("Random construction time", randomTime, observation_mode::AVERAGE_VALUE);
	observe_variable("Random/Lower bound factor", lowerBoundRandomFactor, observation_mode::AVERAGE_VALUE);
	observe_variable("Heuristic construction variety", heuristicVariety, observation_mode::AVERAGE_VALUE);
	observe_variable("Heuristic construction penalty", heuristicPenalty, observation_mode::AVERAGE_VALUE);
	observe_variable("Heuristic construction time", heuristicTime, observation_mode::AVERAGE_VALUE);
	observe_variable("Heuristic/Lower bound factor", lowerBoundHeuristicFactor, observation_mode::AVERAGE_VALUE);
	observe_variable("Heuristic/Random variety factor", varietyFactor, observation_mode::AVERAGE_VALUE);
	observe_variable("Heuristic/Random penaltyFactor", penaltyFactor, observation_mode::AVERAGE_VALUE);

	lowerBound = graph->lowerBound();
	benchmark("initial solution construction", *numberOfRuns) {

		beginTime = chrono::high_resolution_clock::now();
		solution = constructRandomSolution(*graph);

		randomTime = chrono::high_resolution_clock::now() - beginTime;
		randomPenalty = graph->totalPenalty(solution);
		lowerBoundRandomFactor = randomPenalty/lowerBound;
		randomVariety = iterativeVariety(*graph, randomSolutions, solution);

		randomSolutions.push_back(solution);

		beginTime = chrono::high_resolution_clock::now();
		solution = constructHeuristicSolution(*graph);

		heuristicTime = chrono::high_resolution_clock::now() - beginTime;
		heuristicPenalty = graph->totalPenalty(solution);
		lowerBoundHeuristicFactor = heuristicPenalty/lowerBound;
		heuristicVariety = iterativeVariety(*graph, heuristicSolutions, solution);

		heuristicSolutions.push_back(solution);

		if (benchmark::current_run > 1) {
			varietyFactor = heuristicVariety/randomVariety;
		} else {
			varietyFactor = 1;
		}
		penaltyFactor = heuristicPenalty/randomPenalty;

	} end_benchmark;

	delete_observers();
	delete graph;

	return 0;
}
