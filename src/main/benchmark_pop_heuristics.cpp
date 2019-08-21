#include "heuristic.h"
#include "benchmark.h"
#include <cstring>

#define DEFAULT_NUMBER_OF_VERTICES 500
#define DEFAULT_NUMBER_OF_RUNS 10
#define DEFAULT_MAX_VERTEX_DEGREE 10
#define DEFAULT_MIN_VERTEX_DEGREE 4
#define DEFAULT_CYCLE 24
#define DEFAULT_GRAPH_MODEL GraphModel::ADJACENCY_LIST
#define DEFAULT_STOP_CRITERIA_PH stop_criteria::numberOfIterations(100)
#define DEFAULT_COMBINE_METHOD CombineMethod::COMBINE_BY_BFS
#define DEFAULT_ELITE_SIZE 1
#define DEFAULT_DIVERSE_SIZE 9
#define DEFAULT_LOCAL_SEARCH_ITERATIONS 50



#define WRONG_ARGUMENTS_EXIT_CODE 1

using namespace std;
using namespace traffic;
using namespace benchmark;

enum GraphModel {
	ADJACENCY_LIST,
	ADJACENCY_MATRIX
};

enum CombineMethod
{
	CROSSOVER,
	COMBINE_BY_BFS
};

void setupExecutionParameters (int argc, char** argv, size_t &numberOfVertices, size_t &minVertexDegree, size_t &maxVertexDegree, unsigned &numberOfRuns, TimeUnit &cycle, GraphModel &graphModel, function<bool(const HeuristicMetrics&)>& stopCriteriaNotMetPH/*, function<bool(const HeuristicMetrics&)>& stopCriteriaNotMetLS*/, CombineMethod &combineMethod, size_t &elitePopulationSize, size_t &diversePopulationSize, size_t &localSearchIterations) {
	numberOfVertices = DEFAULT_NUMBER_OF_VERTICES;
	minVertexDegree = DEFAULT_MIN_VERTEX_DEGREE;
	maxVertexDegree = DEFAULT_MAX_VERTEX_DEGREE;
	numberOfRuns = DEFAULT_NUMBER_OF_RUNS;
	cycle = DEFAULT_CYCLE;
	graphModel = DEFAULT_GRAPH_MODEL;
	stopCriteriaNotMetPH = DEFAULT_STOP_CRITERIA_PH;
	combineMethod = DEFAULT_COMBINE_METHOD;
	elitePopulationSize = DEFAULT_ELITE_SIZE;
	diversePopulationSize = DEFAULT_DIVERSE_SIZE;
	localSearchIterations = DEFAULT_LOCAL_SEARCH_ITERATIONS;

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
				stopCriteriaNotMetPH = stop_criteria::numberOfIterationsWithoutImprovement(numberOfIterations);

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
				stopCriteriaNotMetPH = stop_criteria::numberOfIterations(numberOfIterations);

			} else if (strcmp(argv[i], "--localSearchIterations") == 0) {

				i++;
				if (i >= argc) {
					cout << "--localSearchIterations argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				localSearchIterations = atoi(argv[i]);
				if (localSearchIterations == 0) {
					cout << "--localSearchIterations argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

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

			}
			else
			if (strcmp(argv[i], "--useCrossover") == 0)
			{

				combineMethod = CombineMethod::CROSSOVER;

			}
			else
			if (strcmp(argv[i], "--useCombineByBfs") == 0)
			{

				combineMethod = CombineMethod::COMBINE_BY_BFS;

			}
			else
			if(strcmp(argv[i], "--diverseSize") == 0)
			{
				i++;

				if (i >= argc)
				{
					cout << "--diverseSize argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				diversePopulationSize = atoi(argv[i]);

				if(diversePopulationSize == 0)
				{
					cout << "--diverseSize argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
			}
			else
			if(strcmp(argv[i], "--eliteSize") == 0)
			{
				i++;

				if (i >= argc)
				{
					cout << "--eliteSize argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				elitePopulationSize = atoi(argv[i]);

				if(elitePopulationSize == 0)
				{
					cout << "--eliteSize argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
			}
			else{
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
	Solution populationalHeuristicSolution;
	size_t numberOfVertices, maxVertexDegree, minVertexDegree, elitePopulationSize, diversePopulationSize;
	unsigned numberOfRuns;
	TimeUnit cycle;
	GraphModel graphModel;
	function<bool(const HeuristicMetrics&)> stopCriteriaNotMetPH;
	Solution (*combineMethodFunction)(const Graph&, const Solution*, const Solution*, int, double);
	TerminalObserver *terminalObserver;
	list<Observer*> observers;
	double avgInitialConstructionPenalty, avgLocalSearchPenalty, avgPHPenalty, avgLowerBound;
	double penaltyFactor, lowerBoundFactor;
	CombineMethod combineMethod;
	chrono::high_resolution_clock::time_point beginPopulationalHeuristic;
	chrono::high_resolution_clock::duration avgPHDuration;
	string formatedAvgPHDuration;
	size_t localSearchIterations;

	setupExecutionParameters(argc, argv, numberOfVertices, minVertexDegree, maxVertexDegree, numberOfRuns, cycle, graphModel, stopCriteriaNotMetPH, combineMethod, elitePopulationSize, diversePopulationSize, localSearchIterations);

	terminalObserver = new TerminalObserver();
	terminalObserver->observeVariable("graph lower bound", avgLowerBound);
	terminalObserver->observeVariable("populational heuristics penalty", avgPHPenalty);
	terminalObserver->observeVariable("populational heuristic/lower bound factor", lowerBoundFactor);
	terminalObserver->observeVariable("populational heuristic duration", formatedAvgPHDuration);
	observers.push_back(terminalObserver);

	avgLowerBound = 0;

	avgPHDuration = chrono::high_resolution_clock::duration(0);
	for (auto o : observers) o->notifyBenchmarkBegun("populational heuristic", numberOfRuns);

	switch(combineMethod)
	{
		case CombineMethod::CROSSOVER:
			combineMethodFunction = &crossover;
			break;
		case CombineMethod::COMBINE_BY_BFS:
			combineMethodFunction = &combineByBfs_aux;
			break;
	}

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

		beginPopulationalHeuristic = chrono::high_resolution_clock::now();
		populationalHeuristicSolution = populationalHeuristic(*graph, elitePopulationSize, diversePopulationSize, localSearchIterations, stopCriteriaNotMetPH, combineMethodFunction);

		avgPHDuration = (avgPHDuration*i + chrono::high_resolution_clock::now() - beginPopulationalHeuristic)/(i+1);
		avgPHPenalty = (avgPHPenalty*i + graph->totalPenalty(populationalHeuristicSolution))/(i+1);
		avgLowerBound = (avgLowerBound*i + graph->lowerBound())/(i+1);

		lowerBoundFactor = avgPHPenalty/avgLowerBound;
		formatedAvgPHDuration = format_chrono_duration(avgPHDuration);

		for (auto o : observers) o->notifyRunEnded();

		delete graphBuilder;
		delete graph;
	}

	for (auto o : observers) o->notifyBenchmarkEnded();

	delete terminalObserver;

	return 0;
}
