# Search Heuristic for the Simplified Traffic Light Problem
Implementations of constructive heuristic, local search heuristic and populational heuristic for the NP-Complete Simplified Traffic Light Problem as described in [this publication](https://www.researchgate.net/publication/244404904_The_Model_and_Properties_of_the_Traffic_Light_Problem)

## Running tests and building
Assertions is being used as the framework for the tests and benchmarks. For information on how to run builds and tests check [Assertions' GitHub repo](https://github.com/rockerbacon/assertions).

## Available Programs
### Benchmark constructive heuristic
This programs benchmarks a purely random construction vs the proposed constructive heuristic, analyzing both variety and penalty. The graphs used for the solutions are generated randomly each run and have the following properties:
- Graph is connected
- Each vertex has a random degree in the closed interval [minVertexDegree, maxVertexDegree] (see usage section)
- Each edge has a weight in the closed interval [1, cycle-1]

#### Usage:
```
benchmark_initial_solution [--vertices number] [--runs number]
```
- _vertices_: number of vertices the random generated graph should have (default 500)
- _runs_: number of times the benchmark should generate a new graph and generate new solutions (default 100)
- _minVertexDegree_: minimum degree a vertex is allowed to have (default 4)
- _maxVertexDegree_: maximum degree a vertex is allowed to have (default 10)
- _cycle_: cycle for the random generated graphs (default 24)

#### KNOWN BUGS
- Scrolling of the terminal can cause the cursor to update information on new lines instead of replacing older lines, causing duplication of information. To avoid, simply ensure that your terminal is clean before running the program;

### Benchmark local search
This program benchmarks the proposed local search heuristic using the proposed constructive heuristic as its initial solution. The graphs used are generated randomly each run and have the following properties:
- Graph is connected
- Each vertex has a random degree in the closed interval [1, maxVertexDegree] (see usage section)
- Each edge has a weight in the closed interval [1, cycle-1] (see usage section)

#### Usage:
```
benchmark_local_search [--vertices number] [--runs number] [--minVertexDegree number] [--maxVertexDegree number] [--useAdjacencyList | --useAdjacencyMatrix] [(--iterationsWithoutImprovement | --iterations) number] [--cycle number]
```
- _vertices_: number of vertices the random generated graphs should have (default 500)
- _runs_: number of times the benchmark should generate a new graph and generate new solutions (default 100)
- _minVertexDegree_: minimum degree a vertex is allowed to have (default 4)
- _maxVertexDegree_: maximum degree a vertex is allowed to have (default 10)
- _useAdjacencyList_ or _useAdjacencyMatrix_: the representation the random generated graphs should use. An adjacency matrix will still build an underlying adjacency list for easier vertex neighborhood iterations (default _useAdjacencyList_)
- _iterationsWithoutImprovement_ or _iterations_: stop criteria for the local search (default _iterations_ 10000)
- _cycle_: cycle for the random generated graphs (default 24)

#### KNOWN BUGS
- Scrolling of the terminal can cause the cursor to update information on new lines instead of replacing older lines, causing duplication of information. To avoid, simply ensure that your terminal is clean before running the program;
