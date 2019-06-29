#include "benchmark.h"
#include <sstream>
#include <iostream>

#define SAVE_CURSOR_POSITION "\033[s"
#define MOVE_CURSOR_TO_SAVED_POSITION "\033[u"
#define MOVE_UP_LINES(numberOfLines) "\033[" << numberOfLines << 'A'
#define CLEAR_LINE "\033[K"

using namespace benchmark;
using namespace std;

Observer::~Observer (void) {}

/*STOPWATCH*/
Stopwatch::Stopwatch (void) {
	this->reset();
}

void Stopwatch::reset (void) {
	this->stopwatchBegin = chrono::high_resolution_clock::now();
	this->lapBegin = this->stopwatchBegin;
}

string Stopwatch::formatedTotalTime (void) const {
	auto timeSinceBeginning = chrono::high_resolution_clock::now() - this->stopwatchBegin;
	return format_chrono_duration(timeSinceBeginning);
}

string Stopwatch::formatedLapTime (void) const {
	auto timeSinceLapBegun = chrono::high_resolution_clock::now() - this->lapBegin;
	return format_chrono_duration(timeSinceLapBegun);
}

void Stopwatch::newLap (void) {
	this->lapBegin = chrono::high_resolution_clock::now();
}
/*STOPWATCH*/

/*TERMINAL OBSERVER*/
TerminalObserver::TerminalObserver (const std::string& benchmarkTitle, unsigned int numberOfRuns) {
	this->benchmarkTitle = benchmarkTitle;
	this->numberOfRuns = numberOfRuns;
}

TerminalObserver::~TerminalObserver (void) {
	for (auto observableVariable : this->variablesToObserve) {
		delete observableVariable;
	}
}

void TerminalObserver::notifyBenchmarkBegun (void) {
	this->stopwatch.reset();
	cout << "Executing " << this->benchmarkTitle << ' ' << this->numberOfRuns << " times..." << endl;
	cout << SAVE_CURSOR_POSITION;
}

void TerminalObserver::notifyRunBegun (void) {
	this->numberOfRuns++;
	this->stopwatch.newLap();
	this->notifyRunUpdate();
}

void TerminalObserver::notifyRunUpdate (void) {
	cout << MOVE_CURSOR_TO_SAVED_POSITION << CLEAR_LINE;
	cout << "\tExecution time: " << this->stopwatch.formatedTotalTime() << endl;
	for (auto observableVariable : this->variablesToObserve) {
		cout << CLEAR_LINE;
		cout << "\t" << observableVariable->getLabel() << ": " << observableVariable->getValue() << endl;
	}
}

void TerminalObserver::notifyRunEnded (void) {
	cout << endl;
}

void TerminalObserver::notifyBenchmarkEnded (void) {
	cout << "Benchmark finished in " << this->stopwatch.formatedTotalTime() << endl;
}
/*TERMINAL OBSERVER*/
