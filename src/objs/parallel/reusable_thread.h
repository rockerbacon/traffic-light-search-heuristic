#pragma once

//#define REUSABLE_THREAD_SPINLOCK

#include <thread>
#include <mutex>
#include <future>
#include <list>
#include <functional>

namespace parallel {

	class reusable_thread {
		private:
			volatile bool running;
			volatile unsigned tasks_count;
			std::mutex mutex;
		#ifndef REUSABLE_THREAD_SPINLOCK
			std::condition_variable notifier;
		#endif
			std::thread thread;
			std::list<std::packaged_task<void()>> task_queue;

		public:
			reusable_thread();
			~reusable_thread();

			std::future<void> exec(const std::function<void()> &task);
			void join();
			bool joinable() const;
	};

	class thread_pile {
		private:
			unsigned number_of_threads;
			std::vector<reusable_thread> threads;
		public:
			typedef decltype(threads)::iterator iterator;
			struct slice_t {
				iterator begin;
				iterator end;

				slice_t(iterator begin, iterator end);

				slice_t slice(unsigned begin, unsigned end);
			};

			thread_pile(unsigned number_of_threads, unsigned call_depth);
			thread_pile(unsigned number_of_threads);
			thread_pile() = default;

			reusable_thread& operator[](unsigned thread_index);
			slice_t depth(unsigned depth);

			slice_t slice (unsigned begin, unsigned end);

			operator slice_t ();
	};

}
