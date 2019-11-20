#include "reusable_thread.h"

using namespace std;
using namespace parallel;

reusable_thread::reusable_thread() :
	running(true),
	has_new_task(false),
	mutex(),
	condition_variable(),
	thread([this]() {
		unique_lock<std::mutex> lock(this->mutex);
		while(this->running) {
			this->condition_variable.wait(lock, [this]() { return this->has_new_task; });
			current_task();
			this->has_new_task = false;
		}
	})
{}

reusable_thread::~reusable_thread() {
	if (this->joinable()) {
		this->join();
	}
}

future<void> reusable_thread::exec(const function<void()> &task_function) {
	packaged_task<void()> task(task_function);
	auto future = task.get_future();

	{
		lock_guard<std::mutex> lock(this->mutex);
		swap(this->current_task, task);
		this->has_new_task = true;
	}
	this->condition_variable.notify_one();

	return future;
}

void reusable_thread::join() {
	this->running = false;
	this->exec({ /* empty task */});
	this->thread.join();
}

bool reusable_thread::joinable() const {
	return this->thread.joinable();
}
