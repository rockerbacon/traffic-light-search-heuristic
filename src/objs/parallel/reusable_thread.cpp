#include "reusable_thread.h"

#include <iostream>

using namespace std;
using namespace parallel;

mutex cerr_mutex;

reusable_thread::reusable_thread() :
	running(true),
	has_new_task(false),
	mutex(),
	condition_variable(),
	thread([this]() {

		cerr_mutex.lock();
		cerr << "initializing thread" << endl;
		cerr_mutex.unlock();

		unique_lock<std::mutex> lock(this->mutex);
		while(this->running) {

			cerr_mutex.lock();
			cerr << "thread waiting for task" << endl;
			cerr_mutex.unlock();

			this->condition_variable.wait(lock, [this]() { return this->has_new_task; });

			cerr_mutex.lock();
			cerr << "thread beginning task execution" << endl;
			cerr_mutex.unlock();

			current_task();
			this->has_new_task = false;

			cerr_mutex.lock();
			cerr << "thread finished task execution" << endl;
			cerr_mutex.unlock();
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
