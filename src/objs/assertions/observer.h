#pragma once

#include <sstream>
#include "assertions/stopwatch.h"
#include <list>
#include <fstream>

namespace benchmark {

	class ObservableVariable {
		public:
			virtual ~ObservableVariable(void) = default;
			virtual std::string get_label (void) = 0;
			virtual std::string get_value (void) = 0;
	};

	template<typename T>
	class ObservableVariableTemplate : public ObservableVariable {
		private:
			const std::string label;
			const T& value;
		public:
			ObservableVariableTemplate (const std::string& label, const T& variable) : value(variable), label(label) {}

			std::string get_label (void) {
				return this->label;
			}

			std::string get_value (void) {
				std::ostringstream str_builder;
				str_builder << this->value;
				return str_builder.str();
			}

	};

	template<typename T>
	class ObservableVariableAverageTemplate : public ObservableVariable {
		private:
			const std::string label;
			const T& value;
			T average;
			unsigned number_of_samples;
		public:
			ObservableVariableAverageTemplate (const std::string& label, const T& variable) : label(label+" (avg)"), value(variable), number_of_samples(0) {}

			std::string get_label(void) {
				return this->label;
			}

			std::string get_value (void) {
				std::ostringstream str_builder;
				if (this->number_of_samples == 0) {
					this->average = this->value;
				} else {
					this->average = (this->average*this->number_of_samples + this->value)/(this->number_of_samples+1);
				}
				this->number_of_samples++;
				str_builder << this->average;
				return str_builder.str();
			}

	};

	template<typename T>
	class ObservableVariableMaximumTemplate : public ObservableVariable {
		private:
			const std::string label;
			const T& value;
			T maximum;
			bool maximum_not_set;
		public:
			ObservableVariableMaximumTemplate (const std::string &label, const T& variable) : label(label+" (max)"), value(variable), maximum_not_set(true) {}
			std::string get_label (void) {
				return this->label;
			}
			std::string get_value (void) {
				std::ostringstream str_builder;
				if (this->maximum_not_set || this->value > this->maximum) {
					this->maximum = this->value;
					this->maximum_not_set = false;
				}
				str_builder << this->maximum;
				return str_builder.str();
			}
	};

	template<typename T>
	class ObservableVariableMinimumTemplate : public ObservableVariable {
		private:
			const std::string label;
			const T& value;
			T minimum;
			bool minimum_not_set;
		public:
			ObservableVariableMinimumTemplate (const std::string &label, const T& variable) : label(label+" (min)"), value(variable), minimum_not_set(true) {}

			std::string get_label (void) {
				return this->label;
			}
			std::string get_value (void) {
				std::ostringstream str_builder;
				if (this->minimum_not_set || this->value < this->minimum) {
					this->minimum = this->value;
					this->minimum_not_set = false;
				}
				str_builder << this->minimum;
				return str_builder.str();
			}
	};


	namespace observation_mode {
		const unsigned CURRENT_VALUE = 0x01;
		const unsigned AVERAGE_VALUE = 0x01 << 1;
		const unsigned MAXIMUM_VALUE = 0x01 << 2;
		const unsigned MINIMUM_VALUE = 0x01 << 3;
	};

	class Observer {
		protected:
			std::list<ObservableVariable*> variables_to_observe;
		public:
			virtual ~Observer (void);
			virtual void notifyBenchmarkBegun (const std::string& benchmarkTitle, unsigned numberOfRuns) = 0;
			virtual void notifyRunBegun (void) = 0;
			virtual void notifyRunEnded (void) = 0;
			virtual void notifyBenchmarkEnded (void) = 0;

			template<typename T>
			void observe_variable(const std::string &variable_label, const T &variable, unsigned observation_mode=observation_mode::CURRENT_VALUE) {
				if (observation_mode & observation_mode::CURRENT_VALUE) {
					ObservableVariableTemplate<T> *observable_variable = new ObservableVariableTemplate<T>(variable_label, variable);
					this->variables_to_observe.push_back(observable_variable);
				}

				if (observation_mode & observation_mode::AVERAGE_VALUE) {
					ObservableVariableAverageTemplate<T> *observable_variable_average = new ObservableVariableAverageTemplate<T>(variable_label, variable);
					this->variables_to_observe.push_back(observable_variable_average);
				}

				if (observation_mode & observation_mode::MAXIMUM_VALUE) {
					ObservableVariableMaximumTemplate<T> *observable_variable_maximum = new ObservableVariableMaximumTemplate<T>(variable_label, variable);
					this->variables_to_observe.push_back(observable_variable_maximum);
				}

				if (observation_mode & observation_mode::MINIMUM_VALUE) {
					ObservableVariableMinimumTemplate<T> *observable_variable_minimum = new ObservableVariableMinimumTemplate<T>(variable_label, variable);
					this->variables_to_observe.push_back(observable_variable_minimum);
				}
			}
	};

	class TerminalObserver : public Observer {
		private:
			unsigned numberOfRuns;
		public:
			TerminalObserver (void) = default;

			void notifyBenchmarkBegun (const std::string& benchmarkTitle, unsigned numberOfRuns);
			void notifyRunBegun (void);
			void notifyRunEnded (void);
			void notifyBenchmarkEnded (void);
	};

	class TextFileObserver : public Observer {
		private:
			std::ofstream outputFile;
			unsigned numberOfRuns;
		public:
			TextFileObserver (const std::string& outputFilePath);

			void notifyBenchmarkBegun (const std::string &benchmarkTitle, unsigned numberOfRuns);
			void notifyRunBegun (void);
			void notifyRunEnded (void);
			void notifyBenchmarkEnded (void);

	};
}
