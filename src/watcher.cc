#include "watcher.h"

Isolate* Watcher::isolate;

uv_thread_t Watcher::thread;
uv_loop_t* Watcher::loop;
uv_timer_t Watcher::timer;
uv_mutex_t* Watcher::locker;
uint64_t Watcher::counter;
uint64_t Watcher::timeout;

uint64_t lastCounter;
uint64_t lastTimeout;


void Watcher::Initialize() {

    Watcher::isolate = Isolate::GetCurrent();

    Watcher::locker = new uv_mutex_t;
    uv_mutex_init(locker);

    Watcher::loop = new uv_loop_t;
    uv_loop_init(Watcher::loop);
    uv_timer_init(Watcher::loop, &Watcher::timer);
    uv_timer_start(&Watcher::timer, &Watcher::Tick, 0, 20);
    uv_thread_create(&Watcher::thread, &Watcher::Run, nullptr);
    
}
void Watcher::Increment() {
    uv_mutex_lock(Watcher::locker);
    Watcher::counter += 1;
    uv_mutex_unlock(Watcher::locker);
}
void Watcher::Start(uint64_t timeout) {
    uv_mutex_lock(Watcher::locker);
    Watcher::counter += 1;
    Watcher::timeout = timeout;
    uv_mutex_unlock(Watcher::locker);
}
void Watcher::Stop() {
    uv_mutex_lock(Watcher::locker);
    Watcher::counter += 1;
    Watcher::timeout = 0;
    uv_mutex_unlock(Watcher::locker);
}

void Watcher::Run(void* arg) {
    uv_run(Watcher::loop, UV_RUN_DEFAULT);
}
void Watcher::Tick(uv_timer_t* timer) {

    // is there a timeout ?
    uint64_t timeout = Watcher::timeout;
    if (timeout == 0){ return; }

    // get underlying user cpu usage
    uv_rusage_t rusage;
    uv_getrusage(&rusage);
    uv_timeval_t utime = rusage.ru_utime;
    uint64_t uticks = TICKS_PER_SEC * utime.tv_sec + utime.tv_usec;

    uv_mutex_lock(Watcher::locker);

    // ensure timeout in the critical section
    timeout = Watcher::timeout;
    if (timeout != 0){ 

        uint64_t counter = Watcher::counter;

        if (counter != lastCounter){
            lastCounter = counter;
            lastTimeout = uticks + (timeout * TICKS_PER_SEC / 1000);
        }
        else {
            if (uticks > lastTimeout){
                Watcher::counter += 1;
                Watcher::timeout = 0;
                Watcher::isolate->TerminateExecution();
            }
        }

    }

    uv_mutex_unlock(Watcher::locker);

}