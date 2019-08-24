#include "assertions/assert.h"
#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "assertions/stopwatch.h"

#define ERROR_TEXT_COLOR "\033[91m"
#define SUCCESS_TEXT_COLOR "\033[92m"
#define DEFAULT_TEXT_COLOR "\033[0m"

std::string assert::test_case_title;
std::stringstream assert::actual_value_str;
std::stringstream assert::expected_value_str;

std::chrono::high_resolution_clock::time_point assert::test_case_start;

bool assert::test_case_succeeded;
bool assert::first_setup_done = false;

struct sigaction signal_action;

using namespace std;
using namespace assert;
using namespace benchmark;

void when_segfault_is_signalled (int signal, siginfo_t *si, void *arg) {
	assert::signal_test_case_failed(segmentation_fault_signalled(), assert::test_case_title);
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

void assert::signal_test_case_failed (const exception& e, const string& test_case_title) {
	auto elapsed_time = chrono::high_resolution_clock::now() - assert::test_case_start;
	cout << ERROR_TEXT_COLOR << "Test case '" << test_case_title << "' failed: " << e.what() << DEFAULT_TEXT_COLOR << " (" << elapsed_time << ")" << endl;
}

void assert::signal_test_case_succeeded (const string& test_case_title) {
	auto elapsed_time = chrono::high_resolution_clock::now() - assert::test_case_start;
	cout << SUCCESS_TEXT_COLOR << "Test case '" << test_case_title << "' OK" << DEFAULT_TEXT_COLOR << " (" << elapsed_time << ")" << endl;
}

void assert::signal_test_case_begun (void) {
	assert::test_case_start = chrono::high_resolution_clock::now();
}
