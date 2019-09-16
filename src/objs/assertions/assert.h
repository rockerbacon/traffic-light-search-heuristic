#pragma once

#include <exception>
#include <sstream>
#include <iostream>
#include <chrono>

#define skip_test_case() throw assert::test_case_interruption()
#define fail_test_case(reason) throw assert::assertion_failed(reason)

#define test_case(str) assert::test_case_title = str;\
	assert::test_case_succeeded = true;\
	assert::run_first_setup_if_needed();\
	assert::signal_test_case_begun();\
	try

#define end_test_case catch (const assert::assertion_failed &e) {\
		assert::test_case_succeeded = false;\
		assert::signal_test_case_failed(e, assert::test_case_title);\
	} catch (const assert::test_case_interruption& e) {\
	} catch (const std::exception &e) {\
		assert::test_case_succeeded = false;\
		assert::signal_test_case_failed(e, assert::test_case_title);\
	}\
	if (assert::test_case_succeeded) {\
		assert::signal_test_case_succeeded(assert::test_case_title);\
	}

#define assert_true(condition) if (!(condition)) {\
		throw assert::assertion_failed("condition was false");\
	}

#define assert_equal(actual_value, expected_value) if (actual_value != expected_value) {\
		assert::actual_value_str << actual_value;\
		assert::expected_value_str << expected_value;\
		throw assert::assertion_failed(assert::actual_value_str, "equal", assert::expected_value_str);\
	}
#define assert_not_equal(actual_value, expected_value) if (actual_value == expected_value) {\
		assert::actual_value_str << actual_value;\
		assert::expected_value_str << expected_value;\
		throw assert::assertion_failed(assert::actual_value_str, "not equal", assert::expected_value_str);\
	}
#define assert_greater_than(actual_value, expected_value) if (actual_value <= expected_value) {\
		assert::actual_value_str << actual_value;\
		assert::expected_value_str << expected_value;\
		throw assert::assertion_failed(assert::actual_value_str, "greater than", assert::expected_value_str);\
	}
#define assert_less_than(actual_value, expected_value) if (actual_value >= expected_value) {\
		assert::actual_value_str << actual_value;\
		assert::expected_value_str << expected_value;\
		throw assert::assertion_failed(assert::actual_value_str, "less than", assert::expected_value_str);\
	}
#define assert_greater_than_or_equal(actual_value, expected_value) if (actual_value < expected_value) {\
		assert::actual_value_str << actual_value;\
		assert::expected_value_str << expected_value;\
		throw assert::assertion_failed(assert::actual_value_str, "greater than or equal", assert::expected_value_str);\
	}
#define assert_less_than_or_equal(actual_value, expected_value) if (actual_value > expected_value) {\
		assert::actual_value_str << actual_value;\
		assert::expected_value_str << expected_value;\
		throw assert::assertion_failed(assert::actual_value_str, "less than or equal", assert::expected_value_str);\
	}



namespace assert {

	extern std::string test_case_title;
	extern std::stringstream actual_value_str;
	extern std::stringstream expected_value_str;
	extern bool test_case_succeeded;
	extern bool first_setup_done;
	extern std::chrono::high_resolution_clock::time_point test_case_start;

	void run_first_setup_if_needed(void);

	class assertion_failed : public std::exception {
		private:
			std::string message;
		public:
			assertion_failed (std::stringstream& actual_value, const std::string& comparator_description, std::stringstream& expected_value);
			assertion_failed (const std::string& reason);

			virtual const char* what (void) const noexcept;
	};

	class test_case_interruption : public std::exception {};

	void signal_test_case_begun(void);
	void signal_test_case_failed(const std::exception &e, const std::string &test_case_title);
	void signal_test_case_succeeded(const std::string &test_case_title);

};
