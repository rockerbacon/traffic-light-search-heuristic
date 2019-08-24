#include "assertions/observer.h"
#include <sstream>
#include <iostream>
#include <ctime>

#define SAVE_CURSOR_POSITION "\033[s"
#define MOVE_CURSOR_TO_SAVED_POSITION "\033[u"
#define MOVE_UP_LINES(numberOfLines) "\033[" << numberOfLines << 'A'
#define CLEAR_LINE "\033[K"

using namespace benchmark;
using namespace std;

Observer::~Observer (void) {
	for (auto observableVariable : this->variables_to_observe) {
		delete observableVariable;
	}
}

/*TERMINAL OBSERVER*/
void TerminalObserver::notifyBenchmarkBegun (const string& benchmarkTitle, unsigned numberOfRuns) {
	this->numberOfRuns = 0;
	cout << "Executing '" << benchmarkTitle << "' " << numberOfRuns << " times..." << endl;
	cout << SAVE_CURSOR_POSITION;
}

void TerminalObserver::notifyRunBegun (void) {
	// do nothing
}

void TerminalObserver::notifyRunEnded (void) {
	this->numberOfRuns++;
	cout << MOVE_CURSOR_TO_SAVED_POSITION << CLEAR_LINE;
	for (auto observable_variable : this->variables_to_observe) {
		cout << CLEAR_LINE;
		cout << "\t" << observable_variable->get_label() << ": " << observable_variable->get_value() << endl;
	}
	cout << endl;
}

void TerminalObserver::notifyBenchmarkEnded (void) {
	// do nothing
}
/*TERMINAL OBSERVER*/

/*TEXT FILE OBSERVER*/
TextFileObserver::TextFileObserver (const string& outputFilePath) {
	this->outputFile.open(outputFilePath, ios::app);
	this->numberOfRuns = 0;
}

void TextFileObserver::notifyBenchmarkBegun (const string& benchmarkTitle, unsigned numberOfRuns) {
	this->numberOfRuns = 0;
	time_t currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
	tm *ptm = localtime(&currentTime);
	char formatedTime[20];
	strftime(formatedTime, 20, "%Y-%m-%d %H:%M:%S", ptm);
	this->outputFile << "Executing '" << benchmarkTitle << "' " << numberOfRuns << " times - " << formatedTime << endl;
}

void TextFileObserver::notifyRunBegun (void) {
	this->numberOfRuns++;
	this->outputFile << "\tRun: " << this->numberOfRuns << endl;
}

void TextFileObserver::notifyRunEnded (void) {
	for (auto observable_variable : this->variables_to_observe) {
		this->outputFile << '\t' << observable_variable->get_label() << ": " << observable_variable->get_value() << endl;
	}
	this->outputFile << endl;
}

void TextFileObserver::notifyBenchmarkEnded (void) {
	// do nothing
}
/*TEXT FILE OBSERVER*/
