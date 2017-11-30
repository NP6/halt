#ifndef MONITOR
#define MONITOR

#include <node.h>
#include <v8.h>

using namespace v8;


class Monitor
{

    public:
        static void Initialize();
        static void Hook(const FunctionCallbackInfo<Value>& args);
        static void Unhook(const FunctionCallbackInfo<Value>& args);

    private:
        static Local<Value> Executor(Local<Promise> promise, Local<Value> resolve, Local<Value> reject);
        static Isolate* isolate;
        static Persistent<Context>* context;
        static Persistent<Function>* callback;

};
    
#endif