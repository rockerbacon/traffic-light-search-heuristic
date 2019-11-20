#pragma once

#include <stdlib.h>
#include <list>
#include <thread>
#include <vector>
#include "reusable_thread.h"

namespace parallel {
	unsigned usable_threads (unsigned number_of_items, unsigned number_of_threads) {
		if (number_of_items < number_of_threads) {
			return 1;
		} else {
			return number_of_threads;
		}
	}

	struct configuration {
		std::vector<reusable_thread>::iterator begin;
		decltype(begin) end;
	};
}

#define using_parallel_configuration(configuration_instance)\
	const ::parallel::configuration &parallel_configuration = configuration_instance;

#define parallel_for(begin, end) {\
\
	auto parallel_number_of_threads = parallel_configuration.end - parallel_configuration.begin; \
	::std::vector<::std::future<void>> parallel_for_futures(parallel_number_of_threads); \
\
	decltype(parallel_number_of_threads) parallel_for_items_per_thread = (end-begin)/parallel_number_of_threads; \
\
	for (auto [thread, thread_i] = ::std::make_tuple(parallel_configuration.begin, 0u); thread < parallel_configuration.end; thread++, thread_i++) { \
\
		auto thread_begin = begin + parallel_for_items_per_thread*thread_i; \
		decltype(thread_begin) thread_end; \
		if (thread_i == parallel_number_of_threads-1) { \
			thread_end = end; \
		} else { \
			thread_end = thread_begin + parallel_for_items_per_thread; \
		} \
\
		parallel_for_futures[thread_i] = thread->exec([&, thread_begin, thread_end](void) { \
			for (auto i = thread_begin; i < thread_end; i++) \

#define end_parallel_for \
		}); \
	} \
\
	for (auto &future : parallel_for_futures) { \
		future.wait(); \
	} \
}

#define for_each_thread {\
\
	auto parallel_number_of_threads = parallel_configuration.end - parallel_configuration.begin; \
	::std::vector<::std::future<void>> parallel_for_futures(parallel_number_of_threads); \
\
	for (auto [thread, thread_i_name] = ::std::make_tuple(parallel_configuration.begin, 0u); thread < parallel_configuration.end; thread++, thread_i_name++) { \
		unsigned thread_i = thread_i_name; \
		parallel_for_futures[thread_i] = thread->exec([&, thread_i](void) \

#define end_for_each_thread \
	);} \
\
	for (auto &future : parallel_for_futures) { \
		future.wait(); \
	} \
\
}

