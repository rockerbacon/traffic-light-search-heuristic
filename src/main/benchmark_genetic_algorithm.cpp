#include "heuristic/heuristic.h"
#include "assertions/benchmark.h"
#include <cstring>

#define DEFAULT_NUMBER_OF_VERTICES 500
#define DEFAULT_NUMBER_OF_RUNS 10
#define DEFAULT_MAX_VERTEX_DEGREE 10
#define DEFAULT_MIN_VERTEX_DEGREE 4
#define DEFAULT_CYCLE 24
#define DEFAULT_GRAPH_MODEL GraphModel::ADJACENCY_LIST
#define DEFAULT_STOP_CRITERIA stop_criteria::numberOfIterations(330)
#define DEFAULT_POP_SIZE 14
#define DEFAULT_MUT_PROB 0.003
#define DEFAULT_COMBINATION_METHOD CombineMethod::CROSSOVER

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

void setupExecutionParameters (int argc, char** argv, size_t &numberOfVertices, size_t &minVertexDegree, size_t &maxVertexDegree, unsigned &numberOfRuns, TimeUnit &cycle, GraphModel &graphModel, int &populationSize, double &mutProb, CombineMethod &combinationMethod, function<bool(const HeuristicMetrics&)> &stopCriteriaNotMet)
{
	numberOfVertices = DEFAULT_NUMBER_OF_VERTICES;
	minVertexDegree = DEFAULT_MIN_VERTEX_DEGREE;
	maxVertexDegree = DEFAULT_MAX_VERTEX_DEGREE;
	numberOfRuns = DEFAULT_NUMBER_OF_RUNS;
	cycle = DEFAULT_CYCLE;
	graphModel = DEFAULT_GRAPH_MODEL;
	populationSize = DEFAULT_POP_SIZE;
	mutProb = DEFAULT_MUT_PROB;
	combinationMethod = DEFAULT_COMBINATION_METHOD;
	stopCriteriaNotMet = DEFAULT_STOP_CRITERIA;

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
				stopCriteriaNotMet = stop_criteria::numberOfIterations(numberOfIterations);

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
			else if (strcmp(argv[i], "--populationSize") == 0)
			{
				i++;
				if (i >= argc)
				{
					cout << "--populationSize argument requires a number >= 2" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				populationSize = atoi(argv[i]);

				if (populationSize < 2)
				{
					cout << "--populationSize argument requires a number >= 2" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

			}
			else if (strcmp(argv[i], "--mutProb") == 0)
			{
				i++;
				if (i >= argc)
				{
					cout << "--mutProb argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				mutProb = atof(argv[i]);

				if(populationSize <= 0)
				{
					cout << "--mutProb argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
			} else if (strcmp(argv[i], "--useCrossover") == 0) {
				combinationMethod = CombineMethod::CROSSOVER;
			} else if (strcmp(argv[i], "--useCombineByBfs") == 0) {
				combinationMethod = CombineMethod::COMBINE_BY_BFS;
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
	Solution solution;
	size_t numberOfVertices, maxVertexDegree, minVertexDegree;
	unsigned numberOfRuns;
	TimeUnit cycle;
	GraphModel graphModel;
	int populationSize;
	Solution (*combinationFunction)(const Graph&, const Solution*, const Solution*, int, double);
	CombineMethod combinationMethod;
	function<bool(const HeuristicMetrics&)> stopCriteriaNotMet;

	double penalty, lowerBound, mutProb;
	double penaltyFactor, lowerBoundFactor;
	chrono::high_resolution_clock::time_point begin;
	chrono::high_resolution_clock::duration duration;

	setupExecutionParameters(argc, argv, numberOfVertices, minVertexDegree, maxVertexDegree, numberOfRuns, cycle, graphModel, populationSize, mutProb, combinationMethod, stopCriteriaNotMet);

	register_observer(new TerminalObserver());

	observe_variable("graph lower bound", lowerBound, observation_mode::AVERAGE_VALUE);
	observe_variable("genetic algorithm penalty", penalty, observation_mode::AVERAGE_VALUE);
	observe_variable("genetic algorithm/lower bound factor", lowerBoundFactor, observation_mode::AVERAGE_VALUE);
	observe_variable("genetic algorithm duration", duration, observation_mode::AVERAGE_VALUE);

	switch(combinationMethod) {
		case CombineMethod::CROSSOVER:
			combinationFunction = &crossover;
			break;
		case CombineMethod::COMBINE_BY_BFS:
			combinationFunction = &combineByBfs_aux;
			break;
	}

	benchmark("genetic algorithm", numberOfRuns) {
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

		begin = chrono::high_resolution_clock::now();
		solution = geneticAlgorithm(*graph, populationSize, mutProb, stopCriteriaNotMet, combinationFunction);

		duration = chrono::high_resolution_clock::now() - begin;
		penalty = graph->totalPenalty(solution);
		lowerBound = graph->lowerBound();

		lowerBoundFactor = penalty/lowerBound;

		delete graphBuilder;
		delete graph;
	} end_benchmark;

	for (auto o : observers) o->notifyBenchmarkEnded();

	delete_observers();

	return 0;
}
