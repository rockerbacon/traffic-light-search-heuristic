#pragma once

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
			std::condition_variable notifier;
			std::thread thread;
			std::list<std::packaged_task<void()>> task_queue;

		public:
			reusable_thread();
			~reusable_thread();

			std::future<void> exec(const std::function<void()> &task);
			void join();
			bool joinable() const;
	};

}
