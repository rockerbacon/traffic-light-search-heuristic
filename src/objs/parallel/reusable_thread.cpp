#include "reusable_thread.h"

using namespace std;
using namespace parallel;

reusable_thread::reusable_thread() :
	running(true),
	tasks_count(0),
	mutex(),
#ifndef REUSABLE_THREAD_SPINLOCK
	notifier(),
#endif
	thread([this]() {
		packaged_task<void()> current_task;
		while(this->running || this->tasks_count > 0) {
		#ifdef REUSABLE_THREAD_SPINLOCK
			while (this->tasks_count == 0) { /* wait for tasks*/ }
		#endif
			{
				unique_lock<std::mutex> lock(this->mutex);
			#ifndef REUSABLE_THREAD_SPINLOCK
				this->notifier.wait(lock, [this]{ return this->tasks_count > 0; });
			#endif
				current_task = std::move(this->task_queue.front());
				this->task_queue.pop_front();
				this->tasks_count--;
			}
			current_task();
		}
	})
{}

reusable_thread::~reusable_thread() {
	if (this->joinable()) {
		this->join();
	}
}

future<void> reusable_thread::exec(const function<void()> &task) {
	packaged_task<void()> packaged_task(task);
	auto future = packaged_task.get_future();
	{
		lock_guard<std::mutex> lock(this->mutex);
		this->task_queue.emplace_back(std::move(packaged_task));
		this->tasks_count++;
	}
#ifndef REUSABLE_THREAD_SPINLOCK
	this->notifier.notify_one();
#endif

	return future;
}

void reusable_thread::join() {
	this->running = false;
	this->exec([]{ /* empty task */ });
	this->thread.join();
}

bool reusable_thread::joinable() const {
	return this->thread.joinable();
}

thread_pile::slice_t::slice_t(iterator begin, iterator end) :
	begin(begin),
	end(end)
{}

thread_pile::slice_t thread_pile::slice_t::slice(unsigned begin, unsigned end) {
	return slice_t{
		this->begin+begin,
		this->begin+end
	};
}

thread_pile::thread_pile(unsigned number_of_threads, unsigned call_depth) :
	number_of_threads(number_of_threads),
	threads(number_of_threads*call_depth)
{}

thread_pile::thread_pile(unsigned number_of_threads) :
	thread_pile(number_of_threads, 1)
{}

reusable_thread& thread_pile::operator[](unsigned thread_index) {
	return this->threads[thread_index];
}

thread_pile::slice_t thread_pile::depth(unsigned depth) {
	auto depth_begin = this->threads.begin()+depth*this->number_of_threads;
	return slice_t{
		depth_begin,
		depth_begin+this->number_of_threads
	};
}

thread_pile::slice_t thread_pile::slice (unsigned begin, unsigned end) {
	return slice_t{
		this->threads.begin()+begin,
		this->threads.begin()+end
	};
}

thread_pile::operator thread_pile::slice_t () {
	return slice_t{
		this->threads.begin(),
		this->threads.begin()+this->number_of_threads
	};
}

