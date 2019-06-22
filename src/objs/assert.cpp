#include "assert.h"
#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#define ERROR_TEXT_COLOR "\033[91m"
#define SUCCESS_TEXT_COLOR "\033[92m"
#define DEFAULT_TEXT_COLOR "\033[0m"

std::string assert::test_case_title;
std::stringstream assert::actual_value_str;
std::stringstream assert::expected_value_str;

assert::TerminalObserver concreteObserver;
assert::Observer& assert::observer = concreteObserver;

bool assert::test_case_succeeded;
bool assert::first_setup_done = false;

struct sigaction signal_action;

using namespace std;
using namespace assert;

void when_segfault_is_signalled (int signal, siginfo_t *si, void *arg) {
	assert::observer.notify_test_case_failed(segmentation_fault_signalled(), assert::test_case_title);
	exit(0);
}

void assert::run_first_setup_if_needed (void) {
	if (!assert::first_setup_done) {
		memset(&signal_action, 0, sizeof(decltype(signal_action)));
		sigemptyset(&signal_action.sa_mask);
		signal_action.sa_sigaction = when_segfault_is_signalled;
		signal_action.sa_flags = SA_SIGINFO;

		sigaction(SIGSEGV, &signal_action, NULL);
		assert::first_setup_done = true;
	}
}

assertion_failed::assertion_failed (stringstream& actual_value, const string& comparator_description, stringstream& expected_value) {
	ostringstream messageStream;
	messageStream << "expected value " << comparator_description << " " << expected_value.rdbuf() << " but got " << actual_value.rdbuf();
	this->message = messageStream.str();
	stringstream().swap(actual_value);
	stringstream().swap(expected_value);
}

assertion_failed::assertion_failed (const string& reason) {
	this->message = reason;
}

const char* assertion_failed::what (void) const noexcept {
	return this->message.c_str();
}

const char* segmentation_fault_signalled::what (void) const noexcept {
	return "segmentation fault, testing cannot continue";
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

void TerminalObserver::notify_test_case_failed (const exception& e, const string& test_case_title) const {
	auto elapsed_time = chrono::high_resolution_clock::now() - this->test_case_start;
	cout << ERROR_TEXT_COLOR << "Test case '" << test_case_title << "' failed: " << e.what() << DEFAULT_TEXT_COLOR << " (" << format_chrono_duration(elapsed_time) << ")" << endl;
}

void TerminalObserver::notify_test_case_succeeded (const string& test_case_title) const {
	auto elapsed_time = chrono::high_resolution_clock::now() - this->test_case_start;
	cout << SUCCESS_TEXT_COLOR << "Test case '" << test_case_title << "' OK" << DEFAULT_TEXT_COLOR << " (" << format_chrono_duration(elapsed_time) << ")" << endl;
}

void TerminalObserver::notify_test_case_begun (void) {
	this->test_case_start = chrono::high_resolution_clock::now();
}
