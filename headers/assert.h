#pragma once

#include <exception>
#include <sstream>
#include <iostream>

#define EXCEPTION_FOR_NEXT_TEST_CASE

#define skip_test_case() throw assert::test_case_interruption()
#define fail_test_case(reason) throw assert::assertion_failed(reason)

/*
#define EXPECT_EXCEPTION_ON_NEXT_TEST(exception_type) #define EXCEPTION_FOR_NEXT_TEST_CASE\
	} catch (const exception_type& e) {\
		assert::expected_excetion_raised(e);
*/

#define test_case(str) assert::test_case_title = str;\
	assert::test_case_succeeded = true;\
	try

#define end_test_case catch (const assert::assertion_failed &e) {\
		assert::test_case_succeeded = false;\
		assert::observer.notify_test_case_failed(e, assert::test_case_title);\
	} catch (const assert::test_case_interruption& e) {\
	/* }\
	 EXCEPTION_FOR_NEXT_TEST_CASE\ */\
	} catch (const std::exception &e) {\
		assert::test_case_succeeded = false;\
		assert::observer.notify_test_case_failed(e, assert::test_case_title);\
	}\
	/*	#undef EXCEPTION_FOR_NEXT_TEST_CASE\
		#define EXCEPTION_FOR_NEXT_TEST_CASE\ */\
	if (assert::test_case_succeeded) {\
		assert::observer.notify_test_case_succeeded(assert::test_case_title);\
	}

#define assert_true(condition) if (!condition) {\
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


	class assertion_failed : public std::exception {
		private:
			std::string message;
		public:
			assertion_failed (std::stringstream& actual_value, const std::string& comparator_description, std::stringstream& expected_value);
			assertion_failed (const std::string& reason);

			virtual const char* what (void) const noexcept;
	};

	class test_case_interruption : public std::exception {};

	class Observer {
		public:
			virtual void notify_test_case_failed(const std::exception& e, const std::string& test_case_title) const = 0;
			virtual void notify_test_case_succeeded (const std::string& test_case_title) const = 0;
	};

	class TerminalObserver : public Observer {
		public:
			virtual void notify_test_case_failed(const std::exception& e, const std::string& test_case_title) const;
			virtual void notify_test_case_succeeded (const std::string& test_case_title) const;
	};

	extern const Observer& observer;

};
