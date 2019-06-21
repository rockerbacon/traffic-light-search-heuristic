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
