#pragma once

#include <thread>
#include <mutex>
#include <future>
#include <functional>

namespace parallel {

	class reusable_thread {
		private:
			volatile bool running;
			volatile bool has_new_task;
			std::mutex mutex;
			std::condition_variable condition_variable;
			std::thread thread;
			std::packaged_task<void()> current_task;

		public:
			reusable_thread();
			~reusable_thread();

			std::future<void> exec(const std::function<void()> &task_function);
			void join();
			bool joinable() const;
	};

}
