#include "monitor.h"

Isolate* Monitor::isolate;
Persistent<Context>* Monitor::context;
Persistent<Function>* Monitor::callback;


void Monitor::Initialize() {

    isolate = Isolate::GetCurrent();
    context = new Persistent<Context>(isolate, isolate->GetCurrentContext());

}
void Monitor::Hook(const FunctionCallbackInfo<Value>& args){

    if (args.Length() != 1) { 
        isolate->ThrowException(
            Exception::RangeError(
                String::NewFromUtf8(isolate, "Wrong number of arguments")
            )
        ); 
        return; 
    }

    if (!args[0]->IsFunction()) { 
        isolate->ThrowException(
            Exception::TypeError(
                String::NewFromUtf8(isolate, "A function is expected")
            )
        ); 
        return; 
    }

    Local<Function> value = Local<Function>::Cast(args[0]);
    callback = new Persistent<Function>(isolate, value);
    isolate->SetPromiseMonitor(Monitor::Executor);

}
void Monitor::Unhook(const FunctionCallbackInfo<Value>& args){

    isolate->SetPromiseMonitor(nullptr);

}

Local<Value> Monitor::Executor(Local<Promise> promise, Local<Value> resolve, Local<Value> reject) {  

    if (promise->CreationContext() == context->Get(isolate)){  return Undefined(isolate); }
    Local<Value> argv[3] = { promise, resolve, reject };
    return callback->Get(isolate)->Call(Undefined(isolate), 3, argv);

}