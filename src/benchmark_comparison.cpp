#include "heuristic.h"
#include "benchmark.h"
#include <list>
#include <cstring>

#define WRONG_ARGUMENTS_EXIT_CODE 1

#define DEFAULT_NUMBER_OF_VERTICES 500
#define DEFAULT_MAX_VERTEX_DEGREE 10
#define DEFAULT_MIN_VERTEX_DEGREE 4
#define DEFAULT_CYCLE 24
#define DEFAULT_GRAPH_MODEL GraphModel::ADJACENCY_LIST
#define DEFAULT_STOP_CRITERIA stop_criteria::executionTime(100ms)
#define DEFAULT_ELITE_POPULATION_SIZE 1
#define DEFAULT_DIVERSE_POPULATION_SIZE 9
#define DEFAULT_LOCAL_SEARCH_ITERATIONS 50
#define DEFAULT_MUTATION_PROBABILITY 0.003
#define DEFAULT_GENETIC_POPULATION_SIZE 14
#define DEFAULT_OUTPUT_FILE "comparison_results.txt"

enum GraphModel { ADJACENCY_LIST, ADJACENCY_MATRIX };

using namespace std;
using namespace traffic;
using namespace benchmark;

void setupExecutionParameters(int argc, char** argv, Vertex &numberOfVertices, Vertex &maxVertexDegree, Vertex &minVertexDegree, TimeUnit &cycle, GraphModel &graphModel, function<bool(const HeuristicMetrics&)> &stopCriteriaNotMet, size_t &elitePopulationSize, size_t &diversePopulationSize, size_t &geneticPopulationSize, double &mutationProbability, unsigned &numberOfLocalSearchIterations, string &outputFilePath) {

	numberOfVertices = DEFAULT_NUMBER_OF_VERTICES;
	maxVertexDegree = DEFAULT_MAX_VERTEX_DEGREE;
	minVertexDegree = DEFAULT_MIN_VERTEX_DEGREE;
	cycle = DEFAULT_CYCLE;
	graphModel = DEFAULT_GRAPH_MODEL;
	stopCriteriaNotMet = DEFAULT_STOP_CRITERIA;
	elitePopulationSize = DEFAULT_ELITE_POPULATION_SIZE;
	diversePopulationSize = DEFAULT_DIVERSE_POPULATION_SIZE;
	geneticPopulationSize = DEFAULT_GENETIC_POPULATION_SIZE;
	mutationProbability = DEFAULT_MUTATION_PROBABILITY;
	numberOfLocalSearchIterations = DEFAULT_LOCAL_SEARCH_ITERATIONS;
	outputFilePath = DEFAULT_OUTPUT_FILE;

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

			} else if (strcmp(argv[i], "--useAdjacencyList") == 0) {

				graphModel = GraphModel::ADJACENCY_LIST;

			} else if (strcmp(argv[i], "--useAdjacencyMatrix") == 0) {

				graphModel = GraphModel::ADJACENCY_MATRIX;

			} else if (strcmp(argv[i], "--elitePopulation") == 0) {

				i++;
				if (i >= argc) {
					cout << "--elitePopulation argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				elitePopulationSize = atoi(argv[i]);

				if (elitePopulationSize <= 0) {
					cout << "--elitePopulation argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

			} else if (strcmp(argv[i], "--diversePopulation") == 0) {

				i++;

				if (i >= argc) {
					cout << "--diversePopulation argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				diversePopulationSize = atoi(argv[i]);

				if (diversePopulationSize <= 0) {
					cout << "--diversePopulation argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

			} else if (strcmp(argv[i], "--geneticPopulation") == 0) {
				i++;
				if (i >= argc)
				{
					cout << "--geneticPopulation argument requires a number >= 2" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				geneticPopulationSize = atoi(argv[i]);

				if (geneticPopulationSize < 2)
				{
					cout << "--geneticPopulation argument requires a number >= 2" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

			} else if (strcmp(argv[i], "--mutationProbability") == 0) {
				i++;
				if (i >= argc)
				{
					cout << "--mutationProbability argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				mutationProbability = atof(argv[i]);

				if(mutationProbability <= 0)
				{
					cout << "--mutationProbability argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

			} else if (strcmp(argv[i], "--seconds") == 0) {

				int seconds;

				i++;
				if (i >= argc) {
					cout << "--seconds argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				seconds = atoi(argv[i]);
				if (seconds <= 0) {
					cout << "--seconds argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				stopCriteriaNotMet = stop_criteria::executionTime(chrono::seconds(seconds));

			} else if (strcmp(argv[i], "--minutes") == 0) {

				int minutes;

				i++;
				if (i >= argc) {
					cout << "--minutes argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				minutes = atoi(argv[i]);
				if (minutes <= 0) {
					cout << "--minutes argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				stopCriteriaNotMet = stop_criteria::executionTime(chrono::minutes(minutes));

			} else if (strcmp(argv[i], "--hours") == 0) {

				int hours;

				i++;
				if (i >= argc) {
					cout << "--hours argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				hours = atoi(argv[i]);
				if (hours <= 0) {
					cout << "--hours argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				stopCriteriaNotMet = stop_criteria::executionTime(chrono::hours(hours));

			} else if (strcmp(argv[i], "--output") == 0) {

				i++;
				if (i >= argc) {
					cout << "--output argument requires a file path" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

				outputFilePath = string(argv[i]);

			} else {
				cout << "unknown parameter " << argv[i] << endl;
				exit(WRONG_ARGUMENTS_EXIT_CODE);
			}


			i++;
		}
	}

}

int main (int argc, char** argv) {

	Vertex numberOfVertices, maxVertexDegree, minVertexDegree;
	TimeUnit cycle;
	GraphModel graphModel;
	function<bool(const HeuristicMetrics&)> stopCriteriaNotMet;
	size_t elitePopulationSize, diversePopulationSize, geneticPopulationSize;
	unsigned numberOfLocalSearchIterations;
	double mutationProbability;
	string outputFilePath;

	TimeUnit graphLowerBound, penalty;
	double penaltyLowerBoundFactor;

	GraphBuilder *graphBuilder;
	Graph *graph;
	Solution solution;

	list<Observer*> observers;
	TerminalObserver terminalObserver;
	TextFileObserver *txtFileObserver;

	setupExecutionParameters(argc, argv, numberOfVertices, maxVertexDegree, minVertexDegree, cycle, graphModel, stopCriteriaNotMet, elitePopulationSize, diversePopulationSize, geneticPopulationSize, mutationProbability, numberOfLocalSearchIterations, outputFilePath);

	terminalObserver.observeVariable("graph lower bound", graphLowerBound);
	terminalObserver.observeVariable("penalty", penalty);
	terminalObserver.observeVariable("penalty/lower bound factor", penaltyLowerBoundFactor);
	observers.push_back(&terminalObserver);

	txtFileObserver = new TextFileObserver(outputFilePath);
	txtFileObserver->observeVariable("graph lower bound", graphLowerBound);
	txtFileObserver->observeVariable("penalty", penalty);
	txtFileObserver->observeVariable("penalty/lower bound factor", penaltyLowerBoundFactor);
	observers.push_back(txtFileObserver);

	graphBuilder = new GraphBuilder(numberOfVertices, minVertexDegree, maxVertexDegree, 1, cycle-1);
	graphBuilder->withCycle(cycle);
	switch(graphModel) {
		case GraphModel::ADJACENCY_MATRIX:
			graph = graphBuilder->buildAsAdjacencyMatrix();
		break;
		case GraphModel::ADJACENCY_LIST:
			graph = graphBuilder->buildAsAdjacencyList();
		break;
	}
	delete graphBuilder;

	graphLowerBound = graph->lowerBound();

	for (auto observer : observers) {
		observer->notifyBenchmarkBegun("local search from random construction", 1);
		observer->notifyRunBegun();
	}
	solution = localSearchHeuristic(*graph, constructRandomSolution(*graph), stopCriteriaNotMet);
	penalty = graph->totalPenalty(solution);
	penaltyLowerBoundFactor = (double)penalty/graphLowerBound;
	for (auto observer : observers) {
		observer->notifyRunEnded();
		observer->notifyBenchmarkEnded();
	}

	for (auto observer : observers) {
		observer->notifyBenchmarkBegun("local search from heuristic construction", 1);
		observer->notifyRunBegun();
	}
	solution = localSearchHeuristic(*graph, constructHeuristicSolution(*graph), stopCriteriaNotMet);
	penalty = graph->totalPenalty(solution);
	penaltyLowerBoundFactor = (double)penalty/graphLowerBound;
	for (auto observer : observers) {
		observer->notifyRunEnded();
		observer->notifyBenchmarkEnded();
	}

	for (auto observer : observers) {
		observer->notifyBenchmarkBegun("scatter search with crossover combination", 1);
		observer->notifyRunBegun();
	}
	solution = populationalHeuristic(*graph, elitePopulationSize, diversePopulationSize, numberOfLocalSearchIterations, stopCriteriaNotMet, &crossover);
	penalty = graph->totalPenalty(solution);
	penaltyLowerBoundFactor = (double)penalty/graphLowerBound;
	for (auto observer : observers) {
		observer->notifyRunEnded();
		observer->notifyBenchmarkEnded();
	}

	for (auto observer : observers) {
		observer->notifyBenchmarkBegun("scatter search with bfs combination", 1);
		observer->notifyRunBegun();
	}
	solution = populationalHeuristic(*graph, elitePopulationSize, diversePopulationSize, numberOfLocalSearchIterations, stopCriteriaNotMet, &combineByBfs_aux);
	penalty = graph->totalPenalty(solution);
	penaltyLowerBoundFactor = (double)penalty/graphLowerBound;
	for (auto observer : observers) {
		observer->notifyRunEnded();
		observer->notifyBenchmarkEnded();
	}

	for (auto observer : observers) {
		observer->notifyBenchmarkBegun("genetic algorithm with crossover combination", 1);
		observer->notifyRunBegun();
	}
	solution = geneticAlgorithm(*graph, geneticPopulationSize, mutationProbability, stopCriteriaNotMet, &crossover);
	penalty = graph->totalPenalty(solution);
	penaltyLowerBoundFactor = (double)penalty/graphLowerBound;
	for (auto observer : observers) {
		observer->notifyRunEnded();
		observer->notifyBenchmarkEnded();
	}

	for (auto observer : observers) {
		observer->notifyBenchmarkBegun("genetic algorithm with bfs combination", 1);
		observer->notifyRunBegun();
	}
	solution = geneticAlgorithm(*graph, geneticPopulationSize, mutationProbability, stopCriteriaNotMet, &combineByBfs_aux);
	penalty = graph->totalPenalty(solution);
	penaltyLowerBoundFactor = (double)penalty/graphLowerBound;
	for (auto observer : observers) {
		observer->notifyRunEnded();
		observer->notifyBenchmarkEnded();
	}

	delete txtFileObserver;
	delete graph;
	return 0;
}
