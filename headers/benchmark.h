#include <chrono>
#include <string>
#include <unordered_map>
#include <sstream>
#include <list>

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
			virtual void notifyBenchmarkBegun (void) = 0;
			virtual void notifyRunBegun (void) = 0;
			virtual void notifyRunUpdate (void) = 0;
			virtual void notifyRunEnded (void) = 0;
			virtual void notifyBenchmarkEnded (void) = 0;
	};

	class TerminalObserver : public Observer {
		private:
			Stopwatch stopwatch;
			unsigned int numberOfRuns;
			std::list<ObservableVariable*> variablesToObserve;
			std::string benchmarkTitle;
		public:
			TerminalObserver (const std::string& benchmarkTitle, unsigned int numberOfRuns);
			~TerminalObserver (void);

			template<typename T>
			void observeVariable (const std::string& variableLabel, const T& variable) {
				ObservableVariableTemplate<T> *observableVariable = new ObservableVariableTemplate<T>(variableLabel, variable);
				this->variablesToObserve.push_back(observableVariable);
			}

			void notifyBenchmarkBegun (void);
			void notifyRunBegun (void);
			void notifyRunUpdate (void);
			void notifyRunEnded (void);
			void notifyBenchmarkEnded (void);
	};

};
