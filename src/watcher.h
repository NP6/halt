#ifndef WATCHER
#define WATCHER

#include <node.h>
#include <uv.h>
#include <v8.h>

#define TICKS_PER_SEC 1000000

using namespace v8;


class Watcher
{

    public:
        static void Initialize();
        static void Increment();
        static void Start(uint64_t timeout);
        static void Stop();

    private:
        static void Run(void* arg);
        static void Tick(uv_timer_t* timer);

        static Isolate* isolate;

	    static uv_thread_t thread;
	    static uv_loop_t* loop;
        static uv_timer_t timer;
        static uv_mutex_t* locker;

        static uint64_t counter;
        static uint64_t timeout;

};
    
#endif