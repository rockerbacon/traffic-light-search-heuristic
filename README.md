# Search Heuristic for the Simplified Traffic Light Problem
Implementations of constructive heuristic, local search heuristic and populational heuristic for the NP-Complete Simplified Traffic Light Problem as described in [this publication](https://www.researchgate.net/publication/244404904_The_Model_and_Properties_of_the_Traffic_Light_Problem)

## Running tests
Assertions is being used as the framework for the tests. For further information check [Assertions' GitHub repo](https://github.com/rockerbacon/assertions).
```
./test.sh all
```

## Building the programs
All .cpp files in the first depth of the folder _src_ will be built as executable programs inside _release_.

### All programs
```
make
```

### Individual programs
```
make release/[program source name]
```
Where _[program source name]_ is the name of the .cpp file in the first depth of _src_, without its extension

## Available Programs
### Benchmark constructive heuristic
This programs benchmarks a purely random construction vs the proposed constructive heuristic, analyzing both variety and penalty. The graphs used for the solutions are generated randomly each run and have the following properties:
- Graph is connected
- Graph has cycle = 20
- Each vertex has a random degree in the closed interval [1, vertices/3] (see usage section)
- Each edge has a weight in the closed interval [1, cycle-1]

Usage:
```
benchmark_initial_solution [--runs number] [--vertices number]
```
- _runs_: number of times the benchmark should generate a new graph and generate new solutions (default 10)
- _vertices_: number of vertices the random generated graph should have (default 10)

#### KNOWN BUGS
- Scrolling of the terminal can cause the cursor to update information on new lines instead of replacing older lines, causing duplication of information. To avoid, simply ensure that your terminal is clean before running the program;
