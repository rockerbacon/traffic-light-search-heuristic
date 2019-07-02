#include "heuristic.h"
#include "benchmark.h"
#include <cstring>

#define DEFAULT_NUMBER_OF_VERTICES 500
#define DEFAULT_NUMBER_OF_RUNS 10
#define DEFAULT_MAX_VERTEX_DEGREE 10
#define DEFAULT_MIN_VERTEX_DEGREE 4
#define DEFAULT_CYCLE 24
#define DEFAULT_GRAPH_MODEL GraphModel::ADJACENCY_LIST
#define DEFAULT_ITERARIONS 2000
#define DEFAULT_POP_SIZE 14
#define DEFAULT_MUT_PROB 0.00225

#define WRONG_ARGUMENTS_EXIT_CODE 1

using namespace std;
using namespace traffic;
using namespace benchmark;

enum GraphModel {
	ADJACENCY_LIST,
	ADJACENCY_MATRIX
};

void setupExecutionParameters (int argc, char** argv, size_t &numberOfVertices, size_t &minVertexDegree, size_t &maxVertexDegree, unsigned &numberOfRuns, TimeUnit &cycle, GraphModel &graphModel, int &nIterations, int &populationSize, double &mutProb)
{
	numberOfVertices = DEFAULT_NUMBER_OF_VERTICES;
	minVertexDegree = DEFAULT_MIN_VERTEX_DEGREE;
	maxVertexDegree = DEFAULT_MAX_VERTEX_DEGREE;
	numberOfRuns = DEFAULT_NUMBER_OF_RUNS;
	cycle = DEFAULT_CYCLE;
	graphModel = DEFAULT_GRAPH_MODEL;
	nIterations = DEFAULT_ITERARIONS;
	populationSize = DEFAULT_POP_SIZE;
	mutProb = DEFAULT_MUT_PROB;

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
				nIterations = numberOfIterations;

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
					cout << "--populationSize argument requires a number greater than 3" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				populationSize = atoi(argv[i]);

				if (populationSize < 4)
				{
					cout << "--populationSize argument requires a number greater than 3" << endl;
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
	Solution geneticAlgorithmSolution;
	size_t numberOfVertices, maxVertexDegree, minVertexDegree;
	unsigned numberOfRuns;
	TimeUnit cycle;
	GraphModel graphModel;
	int nIterations, populationSize;
	TerminalObserver *terminalObserver;
	list<Observer*> observers;
	double avgGAPenalty, avgLowerBound, mutProb;
	double penaltyFactor, lowerBoundFactor;
	chrono::high_resolution_clock::time_point beginGeneticAlgorithm;
	chrono::high_resolution_clock::duration avgGADuration;
	string formatedAvgGADuration;

	setupExecutionParameters(argc, argv, numberOfVertices, minVertexDegree, maxVertexDegree, numberOfRuns, cycle, graphModel, nIterations, populationSize, mutProb);

	terminalObserver = new TerminalObserver("genetic algorithm", numberOfRuns);
	terminalObserver->observeVariable("graph lower bound", avgLowerBound);
	terminalObserver->observeVariable("genetic algorithm penalty", avgGAPenalty);
	terminalObserver->observeVariable("genetic algorithm/lower bound factor", lowerBoundFactor);
	terminalObserver->observeVariable("genetic algorithm duration", formatedAvgGADuration);
	observers.push_back(terminalObserver);

	avgLowerBound = 0;

	avgGADuration = chrono::high_resolution_clock::duration(0);
	for (auto o : observers) o->notifyBenchmarkBegun();

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

		beginGeneticAlgorithm = chrono::high_resolution_clock::now();
		geneticAlgorithmSolution = geneticAlgorithm(*graph, populationSize, nIterations, mutProb);

		avgGADuration = (avgGADuration*i + chrono::high_resolution_clock::now() - beginGeneticAlgorithm)/(i+1);
		avgGAPenalty = (avgGAPenalty*i + graph->totalPenalty(geneticAlgorithmSolution))/(i+1);
		avgLowerBound = (avgLowerBound*i + graph->lowerBound())/(i+1);

		lowerBoundFactor = avgGAPenalty/avgLowerBound;
		formatedAvgGADuration = format_chrono_duration(avgGADuration);

		for (auto o : observers) {
			o->notifyRunUpdate();
			o->notifyRunEnded();
		}

		delete graphBuilder;
		delete graph;
	}

	for (auto o : observers) o->notifyBenchmarkEnded();

	delete terminalObserver;

	return 0;
}
