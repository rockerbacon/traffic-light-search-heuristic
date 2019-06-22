#include "benchmark.h"
#include <sstream>
#include <iostream>

#define SAVE_CURSOR_POSITION "\033[s"
#define MOVE_CURSOR_TO_SAVED_POSITION "\033[u"
#define MOVE_UP_LINES(numberOfLines) "\033[" << numberOfLines << 'A'
#define CLEAR_LINE "\033[K"

using namespace benchmark;
using namespace std;

/*STOPWATCH*/
Stopwatch::Stopwatch (void) {
	this->reset();
}

void Stopwatch::reset (void) {
	this->stopwatchBegin = chrono::high_resolution_clock::now();
	this->lapBegin = this->stopwatchBegin;
}

template<typename Rep, typename Period=std::ratio<1>>
string format_chrono_duration (chrono::duration<Rep, Period> duration) {
	ostringstream str_builder;
	auto hours = chrono::duration_cast<chrono::hours>(duration);
	duration -= chrono::duration_cast<decltype(duration)>(hours);
	auto minutes = chrono::duration_cast<chrono::minutes>(duration);
	duration -= chrono::duration_cast<decltype(duration)>(minutes);
	auto seconds = chrono::duration_cast<chrono::seconds>(duration);
	duration -= chrono::duration_cast<decltype(duration)>(seconds);
	auto milliseconds = chrono::duration_cast<chrono::milliseconds>(duration);
	duration -= chrono::duration_cast<decltype(duration)>(milliseconds);
	auto microseconds = chrono::duration_cast<chrono::microseconds>(duration);

	auto hours_count = hours.count();
	auto minutes_count = minutes.count();
	auto seconds_count = seconds.count();
	auto milliseconds_count = milliseconds.count();
	auto microseconds_count = microseconds.count();
	if (hours_count > 0) {
		str_builder << hours_count << 'h';
	}
	if (minutes_count > 0) {
		if (str_builder.tellp() > 0) {
			str_builder << ' ';
		}
		str_builder << minutes_count << 'm';
	}
	if (seconds_count > 0) {
		if (str_builder.tellp() > 0) {
			str_builder << ' ';
		}
		str_builder << seconds_count << 's';
	}
	if (milliseconds_count > 0) {
		if (str_builder.tellp() > 0) {
			str_builder << ' ';
		}
		str_builder << milliseconds_count << "ms";
	}
	if (microseconds_count > 0) {
		if (str_builder.tellp() > 0) {
			str_builder << ' ';
		}
		str_builder << microseconds_count << "us";
	}
	if (str_builder.tellp() == 0) {
		str_builder << "0us";
	}
	return str_builder.str();
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
