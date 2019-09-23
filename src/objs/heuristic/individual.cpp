#include "heuristic/population.h"

using namespace heuristic;
using namespace traffic;

Individual::Individual(const Solution &solution, TimeUnit penalty, TimeUnit minimumDistance) :
	solution(solution),
	penalty(penalty),
	minimumDistance(minimumDistance)
{}

Individual::Individual(Vertex numberOfVertices) :
	solution(numberOfVertices),
	penalty(-1),
	minimumDistance(-1)
{}

bool Individual::operator<(const Individual &other) const {
	return this->penalty < other.penalty;
}
