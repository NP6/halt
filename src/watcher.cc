#include "watcher.h"


	Watcher::Watcher(Isolate* isolate) : isolate_(isolate), status(IDLE) {

		int result;

		locker = new uv_mutex_t;
		CHECK(locker);

		result = uv_mutex_init(locker);
		CHECK_EQ(0, result);

		loop = new uv_loop_t;
		CHECK(loop);

		result = uv_loop_init(loop);
		CHECK_EQ(0, result);

		result = uv_async_init(loop, &launcher, &Watcher::Launch);
		CHECK_EQ(0, result);

		result = uv_timer_init(loop, &idleTimer);
		CHECK_EQ(0, result);

		result = uv_timer_start(&idleTimer, [](uv_timer_t*){}, 0, UINT_MAX);
		CHECK_EQ(0, result);

		result = uv_timer_init(loop, &timeoutTimer);
		CHECK_EQ(0, result);

		result = uv_thread_create(&thread, &Watcher::Run, this);
		CHECK_EQ(0, result);

	}


	void Watcher::Start(uint64_t timeout){
		uv_mutex_lock(locker);
		start(timeout);
		uv_mutex_unlock(locker);
	}
	Watcher::STATUS Watcher::Clear(){
		uv_mutex_lock(locker);
		Watcher::STATUS status = clear();
		uv_mutex_unlock(locker);
		return status;
	}



	void Watcher::start(uint64_t timeout){
		if (status == STARTED || status == RUNNING){ clear(); }
		status = STARTED;
		launcher.data = (void*)timeout;
		uv_async_send(&launcher);
	}
	Watcher::STATUS Watcher::clear(){
		Watcher::STATUS current = status;
		status = IDLE;
		uv_timer_stop(&timeoutTimer);
		return current;
	}

	void Watcher::Run(void* arg) {
		Watcher* wd = static_cast<Watcher*>(arg);
		uv_run(wd->loop, UV_RUN_DEFAULT);
	}
	void Watcher::Launch(uv_async_t* async) {

		Watcher* w = node::ContainerOf(&Watcher::launcher, async);
		uint64_t timeout = (uint64_t)async->data;

		// critical section
		uv_mutex_lock(w->locker);
		if (w->status == STARTED){
			w->status = RUNNING;
			uv_timer_start(&w->timeoutTimer, &Watcher::Timeout, timeout, 0);
		}
		uv_mutex_unlock(w->locker);
	}
	void Watcher::Timeout(uv_timer_t* timer) {
		Watcher* w = node::ContainerOf(&Watcher::timeoutTimer, timer);

		// critical section
		uv_mutex_lock(w->locker);
		if (w->status == RUNNING){
			w->status = TERMINATED;
			V8::TerminateExecution(w->isolate_);
		}
		uv_mutex_unlock(w->locker);
	}
