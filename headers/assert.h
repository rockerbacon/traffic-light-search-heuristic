#pragma once

#include <exception>
#include <sstream>
#include <iostream>
#include <functional>
#include <memory>

namespace assertion {

	class assert_error : public std::exception {
		private:
			std::string message;
		public:
			assert_error (const std::string& actual_value, const std::string& comparator_description, const std::string& reference_value);
			assert_error (const std::string& comparator_description);

			const char* what (void) const noexcept;
	};

	template<typename T>
	class comparator {
		private:
			std::function<bool(const T&, const T&)> comparison_function;
			std::string description;

			inline int add_to_chain (const decltype(comparator::comparison_function)& comparison_function) {
				int index = this->function_chain.size();
				this->function_chain.push_back(comparison_function);
				return index;
			}
		public:
			inline comparator (const std::string& description, const std::function<bool(const T&, const T&)>& comparison_function) {
				this->description = description;
				this->comparison_function = comparison_function;
			}

			inline bool operator() (const T& actual_value, const T& reference_value) const {
				return this->comparison_function(actual_value, reference_value);
			}

			inline const std::string& get_description (void) const {
				return this->description;
			}

			inline comparator& _not (void) {
				this->description = "not " + this->description;
				this->comparison_function = [=](const T& actual_value, const T& reference_value) -> bool {
					return !this->comparison_function(actual_value, reference_value);
				};
				return *this;
			}
			inline comparator& _and (const comparator<T>& c) {

				this->comparison_function = [=](const T& actual_value, const T& reference_value) {
					return this->comparison_function(actual_value, reference_value) && c.comparison_function(actual_value, reference_value);
				};

				this->description += " and " + c.get_description();

				return *this;
			}
			inline comparator& _or (const comparator<T>& c) {

				this->comparison_function = [=](const T& actual_value, const T& reference_value) {
					return this->comparison_function(actual_value, reference_value) || c.comparison_function(actual_value, reference_value);
				};

				this->description += " or " + c.get_description();

				return *this;
			}

			inline comparator operator&& (const comparator<T>& c) const {
				comparator<T> new_c = *this;
				new_c._and(c);
				return new_c;
			}
			inline comparator operator! (void) const {
				comparator<T> new_c = *this;
				new_c._not();
				return new_c;
			}
			inline comparator operator|| (const comparator<T>& c) const {
				comparator<T> new_c = *this;
				new_c._or(c);
				return new_c;
			}
	};

	template<typename T> constexpr comparator<T> equals() {
		return comparator<T>("equal", [](const T& actual_value, const T& reference_value) -> bool {
			return actual_value == reference_value;
		});
	}
	template<typename T> constexpr comparator<T> less_than() {
		return comparator<T>("less than", [](const T& actual_value, const T& reference_value) -> bool {
			return actual_value < reference_value;
		});
	}
	template<typename T> constexpr comparator<T> greater_than() {
		return comparator<T>("greater than", [](const T& actual_value, const T& reference_value) -> bool {
			return actual_value > reference_value;
		});
	}

	/*
	 * Function for asserting test values
	 *
	 * @param actual_value: value to be tested
	 * @param comparator: comparator object used for testing actual_value against expected_value
	 * @param expected_value: value used for testing actual_value
	 *
	 * @throw assert_error: Thrown when actual_value fails assertion
	 *
	 */
	template<typename T>
	inline void assert (const T& actual_value, const comparator<T>& c, const T& expected_value, bool verbose=true) {
		if (!c(actual_value, expected_value)) {
			if (verbose) {
				std::ostringstream	actual_value_str,
									expected_value_str;
				actual_value_str << actual_value;
				expected_value_str << expected_value;
				throw assert_error(actual_value_str.str(), c.get_description(), expected_value_str.str());
			} else {
				throw assert_error(c.get_description());
			}
		}
	}

	void assert_fail(const std::string& message);

};
