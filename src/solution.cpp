#include "traffic_graph.h"
#include <algorithm>
#include <random>
#include <chrono>

using namespace traffic;
using namespace std;
Solution::Solution(size_t numberOfVertices) {
	this->verticeTimings = new TimeUnit[numberOfVertices];
	for (size_t i = 0; i < numberOfVertices; i++) {
		this->verticeTimings[i] = 0;
	}
}

Solution::~Solution (void) {
	delete this->verticeTimings;
}

void Solution::setTiming (Vertice vertice, TimeUnit timing) {
	this->verticeTimings[vertice] = timing;
}

TimeUnit Solution::getTiming (Vertice vertice) {
	return this->verticeTimings[vertice];
}



Solution* traffic::constructSolution (const Graph& graph) {
	Solution* solution = new Solution(graph.getNumberOfVertices());
	vector<Vertice> unvisitedVertices(graph.getNumberOfVertices());
	Vertice vertice1, vertice2;

	random_device seeder;
	mt19937 randomEngine(seeder());

	uniform_int_distribution<Vertice> verticePicker;
	uniform_int_distribution<TimeUnit> timmingPicker(0, graph.getCycle());

	for (Vertice i = 0; i < graph.getNumberOfVertices(); i++) {
		unvisitedVertices.push_back(i);
	}
	shuffle(unvisitedVertices.begin(), unvisitedVertices.end(), randomEngine);

	randomEngine = mt19937(seeder());
	while (unvisitedVertices.size() > 0) {

		vertice1 = unvisitedVertices.back();
		unvisitedVertices.pop_back();

		verticePicker = uniform_int_distribution<Vertice>(0, unvisitedVertices.size());
		vertice2 = unvisitedVertices[verticePicker(randomEngine)];

		solution->setTiming(vertice1, timmingPicker(randomEngine));
		solution->setTiming(vertice2, timmingPicker(randomEngine));
	}

	return solution;
}
