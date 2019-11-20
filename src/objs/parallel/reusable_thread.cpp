#include "reusable_thread.h"

using namespace std;
using namespace parallel;

reusable_thread::reusable_thread() :
	running(true),
	tasks_count(0),
	mutex(),
	notifier(),
	thread([this]() {
		packaged_task<void()> current_task;
		while(this->running || this->tasks_count > 0) {
			{
				unique_lock<std::mutex> lock(this->mutex);
				this->notifier.wait(lock, [this]() { return this->tasks_count > 0; });
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
	future<void> future;
	{
		lock_guard<std::mutex> lock(this->mutex);
		this->task_queue.emplace_back(task);
		future = this->task_queue.back().get_future();
		this->tasks_count++;
	}

	this->notifier.notify_one();

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
