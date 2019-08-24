#pragma once

#include <list>
#include "assertions/observer.h"

#define benchmark(title, runs) \
	for (auto o : benchmark::observers) {\
		o->notifyBenchmarkBegun(title, runs);\
	}\
	benchmark::benchmark_duration = std::chrono::high_resolution_clock::duration(0);\
   	for (benchmark::current_run = 0; benchmark::current_run < runs; benchmark::current_run++) {\
		for (auto o: benchmark::observers) {\
			o->notifyRunBegun();\
		}\
		benchmark::run_begin = std::chrono::high_resolution_clock::now();

#define end_benchmark \
		benchmark::run_duration = std::chrono::high_resolution_clock::now() - benchmark::run_begin;\
		benchmark::benchmark_duration += benchmark::run_duration;\
		for (auto o: benchmark::observers) {\
			o->notifyRunEnded();\
		}\
	}\
	for (auto o: benchmark::observers) {\
		o->notifyBenchmarkEnded();\
	}

namespace benchmark {
	extern std::list<Observer*> observers;
	extern unsigned current_run;
	extern std::chrono::high_resolution_clock::time_point run_begin;
	extern std::chrono::high_resolution_clock::duration run_duration;
	extern std::chrono::high_resolution_clock::duration benchmark_duration;

	void register_observer(Observer *Observer);
	void delete_observers(void);

	template<typename T>
	void observe_variable(const std::string &variable_label, const T& variable, unsigned observation_mode=observation_mode::CURRENT_VALUE) {
		for (auto o : observers) {
			o->observe_variable(variable_label, variable, observation_mode);
		}
	}

}
