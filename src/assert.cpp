#include "assert.h"

using namespace std;
using namespace assertion;

assert_error::assert_error (const string& actual_value, const string& comparator_description, const string& reference_value) {
	this->message = "Assertion failed: expected value " + comparator_description + " " + reference_value + " but got " + actual_value;
}

assert_error::assert_error (const string& comparator_description) {
	this->message = "Assertion failed on '" + comparator_description + "'";
}

const char* assert_error::what (void) const noexcept {
	return this->message.c_str();
}

void assertion::assert_fail(const std::string& message) {
	throw assert_error(message);
}