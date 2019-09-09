#pragma once

#include <stdlib.h>
#include <list>
#include <thread>

namespace parallel {
	unsigned usable_threads (unsigned number_of_items, unsigned number_of_threads) {
		if (number_of_items < number_of_threads) {
			return 1;
		} else {
			return number_of_threads;
		}
	}
}

#define parallel_for(begin, end, number_of_threads) {\
\
	decltype(number_of_threads) parallel_for_available_threads = number_of_threads; \
	std::thread *parallel_for_executing_threads; \
\
	parallel_for_executing_threads = new std::thread[parallel_for_available_threads]; \
\
	auto parallel_for_items_per_thread = (end-begin)/parallel_for_available_threads; \
\
	for (auto thread_i = 0; thread_i < parallel_for_available_threads; thread_i++) { \
\
		auto thread_begin = begin + parallel_for_items_per_thread*thread_i; \
		decltype(thread_begin) thread_end; \
		if (thread_i == parallel_for_available_threads-1) { \
			thread_end = end; \
		} else { \
			thread_end = thread_begin + parallel_for_items_per_thread; \
		} \
\
		parallel_for_executing_threads[thread_i] = std::thread( [&, thread_i, thread_begin, thread_end](void) { \
			for (auto i = thread_begin; i < thread_end; i++) \

#define end_parallel_for \
		}); \
	} \
\
	for (auto i = 0; i < parallel_for_available_threads; i++) { \
		parallel_for_executing_threads[i].join(); \
	} \
	delete [] parallel_for_executing_threads; \
}
