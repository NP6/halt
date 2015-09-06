#ifndef WATCHER_H
#define WATCHER_H

#include <node.h>
#include <uv.h>
#include <v8.h>
#include <util-inl.h>

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
	Watcher::STATUS Clear();


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


	~Watcher(){
		delete locker;
		delete loop;
	};

	void start(uint64_t timeout);
	Watcher::STATUS clear();

	static void Run(void* arg);
	static void Launch(uv_async_t* async);
	static void Timeout(uv_timer_t* timer);


};


#endif
