#include <chrono>
#include <string>
#include <unordered_map>
#include <sstream>
#include <list>
#include <iostream>
#include <fstream>

namespace benchmark {
	class Stopwatch {
		private:
			std::chrono::time_point<std::chrono::high_resolution_clock> lapBegin;
			std::chrono::time_point<std::chrono::high_resolution_clock> stopwatchBegin;
		public:
			Stopwatch (void);

			void reset (void);
			std::string formatedTotalTime (void) const;
			std::string formatedLapTime (void) const;
			void newLap (void);
	};


	class ObservableVariable {
		public:
			virtual std::string getLabel (void) = 0;
			virtual std::string getValue (void) = 0;
	};

	template<typename T>
	class ObservableVariableTemplate : public ObservableVariable {
		private:
			const std::string label;
			const T& variable;
		public:
			ObservableVariableTemplate (const std::string& label, const T& variable) : variable(variable), label(label) {}

			std::string getLabel (void) {
				return this->label;
			}

			std::string getValue (void) {
				std::ostringstream strBuilder;
				strBuilder << this->variable;
				return strBuilder.str();
			}

	};

	class Observer {
		public:
			virtual ~Observer (void);
			virtual void notifyBenchmarkBegun (const std::string& benchmarkTitle, unsigned numberOfRuns) = 0;
			virtual void notifyRunBegun (void) = 0;
			virtual void notifyRunEnded (void) = 0;
			virtual void notifyBenchmarkEnded (void) = 0;
	};

	class TerminalObserver : public Observer {
		private:
			Stopwatch stopwatch;
			std::list<ObservableVariable*> variablesToObserve;
			unsigned numberOfRuns;
		public:
			TerminalObserver (void) = default;
			~TerminalObserver (void);

			template<typename T>
			void observeVariable (const std::string& variableLabel, const T& variable) {
				ObservableVariableTemplate<T> *observableVariable = new ObservableVariableTemplate<T>(variableLabel, variable);
				this->variablesToObserve.push_back(observableVariable);
			}

			void notifyBenchmarkBegun (const std::string& benchmarkTitle, unsigned numberOfRuns);
			void notifyRunBegun (void);
			void notifyRunEnded (void);
			void notifyBenchmarkEnded (void);
	};

	class TextFileObserver : public Observer {
		private:
			std::ofstream outputFile;
			std::list<ObservableVariable*> variablesToObserve;
			unsigned numberOfRuns;
		public:
			TextFileObserver (const std::string& outputFilePath);
			~TextFileObserver (void);

			template<typename T>
			void observeVariable (const std::string& variableLabel, const T& variable) {
				ObservableVariableTemplate<T> *observableVariable = new ObservableVariableTemplate<T>(variableLabel, variable);
				this->variablesToObserve.push_back(observableVariable);
			}

			void notifyBenchmarkBegun (const std::string &benchmarkTitle, unsigned numberOfRuns);
			void notifyRunBegun (void);
			void notifyRunEnded (void);
			void notifyBenchmarkEnded (void);

	};

	template<typename Rep, typename Period=std::ratio<1>>
	std::string format_chrono_duration (std::chrono::duration<Rep, Period> duration) {
		std::ostringstream str_builder;
		auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
		duration -= std::chrono::duration_cast<decltype(duration)>(hours);
		auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
		duration -= std::chrono::duration_cast<decltype(duration)>(minutes);
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
		duration -= std::chrono::duration_cast<decltype(duration)>(seconds);
		auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
		duration -= std::chrono::duration_cast<decltype(duration)>(milliseconds);
		auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);

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

};
