#include "assert.h"
#include <iostream>
#include <stdio.h>

#define ERROR_TEXT_COLOR "\033[91m"
#define SUCCESS_TEXT_COLOR "\033[92m"
#define DEFAULT_TEXT_COLOR "\033[0m"

std::string assert::test_case_title;
std::stringstream assert::actual_value_str;
std::stringstream assert::expected_value_str;

assert::TerminalObserver concreteObserver;
const assert::Observer& assert::observer = concreteObserver;

bool assert::test_case_succeeded;

using namespace std;
using namespace assert;

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

void TerminalObserver::notify_test_case_failed (const exception& e, const string& test_case_title) const {
	cout << ERROR_TEXT_COLOR << "Test case '" << test_case_title << "' failed: " << e.what() << DEFAULT_TEXT_COLOR << endl;
}

void TerminalObserver::notify_test_case_succeeded (const string& test_case_title) const {
	cout << SUCCESS_TEXT_COLOR << "Test case '" << test_case_title << "' OK" << DEFAULT_TEXT_COLOR << endl;
}
