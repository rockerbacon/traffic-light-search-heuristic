#include <iostream>
#include <string>
#include <stdlib>

#include "heuristic.h"
#include "stopwatch.h"

#define DEFAULT_NUMBER_OF_VERTICES 10
#define DEFAULT_NUMBER_OF_RUNS 10

#define WRONG_ARGUMENTS_EXIT_CODE 1

using namespace std;
using namespace traffic;
using namespace benchmark;

void setupExecutionParameters (int argc, char** argv, size_t &numberOfVertices, unsigned numberOfRuns) {
	if (argc > 1) {
		int i = 1;
		while (i < argc) {
			if (strcmp(argv[i], "--vertices")) {
				i++;
				if (i >= argv[i]) {
					cout << "--vertices argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				numberOfVertices = atoi(argv[i]);
				if (numberOfVertices == 0) {
					cout << "--vertices argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
			} else if (strcmp(argv[i], "--runs")) {
				i++;
				if (i >= argv[i]) {
					cout << "--runs argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}
				numberOfRuns = atoi(argv[i]);
				if (numberOfRuns == 0) {
					cout << "--runs argument requires a number greater than 0" << endl;
					exit(WRONG_ARGUMENTS_EXIT_CODE);
				}

			}
			i++;
		}
	} else {
		numberOfVertices = DEFAULT_NUMBER_OF_VERTICES;
		numberOfRuns = DEFAULT_NUMBER_OF_RUNS;
	}
}

int main (int argc, char** argv) {

	GraphBuilder graphBuilder;
	Stopwatch stopwatch;
	size_t numberOfVertices;
	unsigned numberOfRuns;

	setupExecutionParameters(arg, argv, numberOfVertices, numberOfRuns);

	for (unsigned i = 0; i < numberOfRuns; i++) {

	}

	return 0;
}
