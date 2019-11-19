#include "population.h"

size_t heuristic::scatterSearchPopulationSize(size_t elitePopulationSize, size_t diversePopulationSize) {
       return 3*(elitePopulationSize+diversePopulationSize)/2;
}

