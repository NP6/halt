#ifndef WATCHER_HALT_H
#define WATCHER_HALT_H

#include <node.h>
#include <uv.h>
#include <v8.h>

#define MICROS_PER_SEC 1000000

using namespace v8;

class Watcher
{

public:

	static enum STATUS
	{
		IDLE,
		STARTED,
		RUNNING,
		TERMINATED
	};

	Watcher(Isolate* isolate);

	void Start(uint64_t timeout);
	Watcher::STATUS	Clear();

	void Ticks(uint64_t newTicksPerMsec);
	int Ticks();
	static void setTicks(Local<String> property,
						Local<Value> value,
						const PropertyCallbackInfo<void>& info);
	static void getTicks(Local<String> property,
						const PropertyCallbackInfo<Value>& info);

private:
	Isolate* isolate_;
	uv_thread_t thread;
	uv_loop_t* loop;
	uv_async_t launcher;
	uv_timer_t idleTimer;
	uv_mutex_t* locker;

	uv_timer_t timeoutTimer;
	STATUS status;
	uint64_t timeout;

	uint64_t clock_start;
	uint64_t consume;
	uint64_t maxCpuTicks;

	~Watcher(){
		delete locker;
		delete loop;
	};

	void start(uint64_t timeout);
	Watcher::STATUS clear();

	static void Run(void* arg);
	static void Launch(uv_async_t* async);
	static void Timeout(uv_timer_t* timer);

	uint64_t getUserCpuTime();
	int ticksPerMsec;

};
#endif
