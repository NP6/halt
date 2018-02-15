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
    
        static void ExecutePromiseInitialization(
            Local<Promise> promise, 
            Local<Value> parent
        );
        
        static void ExecutePromiseResolution(
            Local<Promise> promise, 
            Local<Value> result
        );

        static void ExecutePromiseRejection(
            Local<Promise> promise, 
            Local<Value> reason
        );

        static Local<Value> ExecutePromiseInstrumentation(
            Local<Promise> promise, 
            Local<Value> resolve, 
            Local<Value> reject,
            Local<Promise> deferred
        );

        static Isolate* isolate;
        static Persistent<Context>* context;
        static Persistent<Function>* monitorInitialize;
        static Persistent<Function>* monitorResolve;
        static Persistent<Function>* monitorReject;
        static Persistent<Function>* monitorInstrumentation;

};
    
#endif