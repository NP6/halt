#include "watcher.h"
#include "helper.h"

Watcher::Watcher(Isolate* isolate) : isolate_(isolate), status(IDLE), ticksPerMsec(1000) {

	int result;
	locker = new uv_mutex_t;
	result = uv_mutex_init(locker);
	loop = new uv_loop_t;

	consume = 0;
	clock_start = 0;

	result = uv_loop_init(loop);

	result = uv_async_init(loop, &launcher, &Watcher::Launch);

	result = uv_timer_init(loop, &idleTimer);
	result = uv_timer_start(&idleTimer, [](uv_timer_t*) {}, 0, 65535);

	result = uv_timer_init(loop, &timeoutTimer);
	result = uv_thread_create(&thread, &Watcher::Run, this);
}

	void Watcher::Start(uint64_t timeout){
		uv_mutex_lock(locker);
		clock_start = getUserCpuTime();
		start(timeout);
		uv_mutex_unlock(locker);
	}
	Watcher::STATUS Watcher::Clear(){
		uv_mutex_lock(locker);
		status = clear();
		uv_mutex_unlock(locker);
		return status;
	}
	void Watcher::start(uint64_t timeout){
		if (status == STARTED || status == RUNNING){ clear(); }
		status = STARTED;
		Watcher::timeout = timeout;
		maxCpuTicks = timeout * ticksPerMsec;
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
		Watcher* w = ContainerOf(&Watcher::launcher, async);
		uint64_t timeout = (uint64_t)async->data;

		// critical section
		uv_mutex_lock(w->locker);
		if (w->status == STARTED){
			w->status = RUNNING;
			// launch Timeout after timeout ms. If the execution is not terminated, Timeout will be called again after 5% timeout ms
			uv_timer_start(&w->timeoutTimer, &Watcher::Timeout, timeout, timeout / 20 + 5);
		}
		uv_mutex_unlock(w->locker);
	}
	void Watcher::Timeout(uv_timer_t* timer) {
		Watcher* w = ContainerOf(&Watcher::timeoutTimer, timer);
		
		// critical section
		uv_mutex_lock(w->locker);
		if (w->status == RUNNING) {
			uint64_t now = w->getUserCpuTime();
			w->consume = now - w->clock_start + 5000;   // add 5000 user cpu tick
			// Check if the engine consumed the max amount of cpu ticks allowed and if it did, then terminate execution
			if (now == 0 || w->consume >= w->maxCpuTicks) {
				w->status = TERMINATED;
				V8::TerminateExecution(w->isolate_);
			}
		}
		uv_mutex_unlock(w->locker);
	}

	// return userCpuTime in milliseconds, 0 if err
	uint64_t Watcher::getUserCpuTime() {
		uv_rusage_t rusage;

		int err = uv_getrusage(&rusage);
		if (err)
			return 0;
		return  (MICROS_PER_SEC * rusage.ru_utime.tv_sec + rusage.ru_utime.tv_usec);
	}

	void Watcher::Ticks(uint64_t newTicksPerMsec) {
		// Block when watcher is not idle
		uv_mutex_lock(locker);
		if (status == IDLE)
			ticksPerMsec = newTicksPerMsec;
		uv_mutex_unlock(locker);
	}

	int Watcher::Ticks() {
		return ticksPerMsec;
	}

	/*
	* Accessors to watcher's tickPerMsec variable, works by sending an object which internalFields contains a pointer on watcher
	*/
	void Watcher::setTicks(Local<String> property,
							Local<Value> value,
							const PropertyCallbackInfo<void>& info) {
		// get the pointer on watcher to access method ticks
		Isolate *isolate = Isolate::GetCurrent();
		Local<Object> obj = Local<Object>::Cast(info.Data());
		Local<External> wrap = Local<External>::Cast(obj->GetInternalField(0));
		void* ptr = wrap->Value();
		
		int newValue = value->Int32Value();
		static_cast<Watcher*>(ptr)->Ticks(newValue);
	}

	void Watcher::getTicks(Local<String> property,
							const PropertyCallbackInfo<Value>& info) {
		Isolate *isolate = Isolate::GetCurrent();
		Local<Object> obj = Local<Object>::Cast(info.Data());
		Local<External> wrap = Local<External>::Cast(obj->GetInternalField(0));
		void* ptr = wrap->Value();

		int value = static_cast<Watcher*>(ptr)->Ticks();
		info.GetReturnValue().Set(value);
	}