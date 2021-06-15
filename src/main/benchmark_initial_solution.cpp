#include "heuristic/heuristic.h"
#include <stopwatch/stopwatch.h>
#include <cpp-benchmark/benchmark.h>
#include <cpp-command-line-interface/command_line_interface.h>

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

cli_main (
	"Benchmark Traffic Light Heuristic",
	"v0.1",
	"Benchmarks scatter search heuristic against purely random solution",

	cli::RequiredArgument<string> inputFilePath("input", "input file");
	cli::create_alias("input", 'i');

	cli::OptionalArgument<unsigned> numberOfRuns(DEFAULT_NUMBER_OF_RUNS, "runs", "number of runs");

	cli::FlagArgument useAdjacencyList("useAdjacencyList");
	cli::FlagArgument useAdjacencyMatrix("useAdjacencyMatrix");
) {

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

	fileInputStream.open(*inputFilePath);
	graphBuilder.read_from_file(fileInputStream);
	fileInputStream.close();

	if (*useAdjacencyMatrix) {
		graph = graphBuilder.buildAsAdjacencyMatrix();
	} else {
		graph = graphBuilder.buildAsAdjacencyList();
	}
	/* CLI ARGUMENTS */

	TerminalObserver terminalObserver;
	register_observers(terminalObserver);

	observe(lowerBound, lower_bound);
	observe_average(randomVariety, random_construction_variety);
	observe_average(randomPenalty, random_construction_penalty);
	observe_average(randomTime, random_construction_time);
	observe_average(lowerBoundRandomFactor, random_construction_lowerbound_factor);

	observe_average(heuristicVariety, heuristic_variety);
	observe_average(heuristicPenalty, heuristic_penalty);
	observe_average(heuristicTime, heuristic_time);
	observe_average(lowerBoundHeuristicFactor, heuristic_lowerbound_factor);

	observe_average(varietyFactor, heuristic_random_variety_factor);
	observe_average(penaltyFactor, heuristic_random_penalty_factor);

	auto run = 0;
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

		if (run > 1) {
			varietyFactor = heuristicVariety/randomVariety;
		} else {
			varietyFactor = 1;
		}
		penaltyFactor = heuristicPenalty/randomPenalty;

		run++;
	}

	delete graph;

	return 0;
} end_cli_main;
