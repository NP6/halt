#include "monitor.h"

Isolate* Monitor::isolate;
Persistent<Context>* Monitor::context;

Persistent<Function>* Monitor::monitorInitialize;
Persistent<Function>* Monitor::monitorResolve;
Persistent<Function>* Monitor::monitorReject;
Persistent<Function>* Monitor::monitorInstrumentation;


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

    if (!args[0]->IsObject()) { 
        isolate->ThrowException(
            Exception::TypeError(
            )
        ); 
        return; 
    }

    // configuration
    Local<Object> configuration = args[0]->ToObject();

    // initialize
    Local<Value> initialize = configuration->Get(String::NewFromUtf8(isolate, "initialize"));
    monitorInitialize = initialize->IsFunction() ? new Persistent<Function>(isolate, Local<Function>::Cast(initialize)) : nullptr;
    isolate->SetPromiseMonitorInitialize(monitorInitialize == nullptr ? nullptr : Monitor::ExecutePromiseInitialization);

    // resolve
    Local<Value> resolve = configuration->Get(String::NewFromUtf8(isolate, "resolve"));
    monitorResolve = resolve->IsFunction() ? new Persistent<Function>(isolate, Local<Function>::Cast(resolve)) : nullptr;
    isolate->SetPromiseMonitorResolve(monitorResolve == nullptr ? nullptr : Monitor::ExecutePromiseResolution);
   
    // reject
    Local<Value> reject = configuration->Get(String::NewFromUtf8(isolate, "reject"));
    monitorReject = reject->IsFunction() ? new Persistent<Function>(isolate, Local<Function>::Cast(reject)) : nullptr;
    isolate->SetPromiseMonitorReject(monitorReject == nullptr ? nullptr : Monitor::ExecutePromiseRejection);

    // instrument
    Local<Value> instrument = configuration->Get(String::NewFromUtf8(isolate, "instrument"));
    monitorInstrumentation = instrument->IsFunction() ? new Persistent<Function>(isolate, Local<Function>::Cast(instrument)) : nullptr;
    isolate->SetPromiseMonitorPerform(monitorInstrumentation == nullptr ? nullptr : Monitor::ExecutePromiseInstrumentation);

}
void Monitor::Unhook(const FunctionCallbackInfo<Value>& args){

    isolate->SetPromiseMonitorInitialize(nullptr);
    isolate->SetPromiseMonitorResolve(nullptr);
    isolate->SetPromiseMonitorReject(nullptr);
    isolate->SetPromiseMonitorPerform(nullptr);

}

void Monitor::ExecutePromiseInitialization(Local<Promise> promise, Local<Value> parent) {

    Local<Context> safe = context->Get(isolate);
    if (promise->CreationContext() == safe){ return; }

    Local<Value> argv[2] = { promise, parent };
    monitorInitialize->Get(isolate)->Call(Undefined(isolate), 2, argv);
  
}

void Monitor::ExecutePromiseResolution(Local<Promise> promise, Local<Value> result) {

    Local<Context> safe = context->Get(isolate);
    if (promise->CreationContext() == safe){ return; }
    if (!result->IsPromise()){ return; }
    if (Local<Promise>::Cast(result)->CreationContext() == safe){ return; }

    Local<Value> argv[2] = { promise, result };
    monitorResolve->Get(isolate)->Call(Undefined(isolate), 2, argv);
  
}

void Monitor::ExecutePromiseRejection(Local<Promise> promise, Local<Value> reason) {

    Local<Context> safe = context->Get(isolate);
    if (promise->CreationContext() == safe){ return; }

    Local<Value> argv[2] = { promise, reason };
    monitorReject->Get(isolate)->Call(Undefined(isolate), 2, argv);
  
}

Local<Value> Monitor::ExecutePromiseInstrumentation(
    Local<Promise> promise, 
    Local<Value> resolve, 
    Local<Value> reject,
    Local<Promise> deferred) {  

    Local<Context> safe = context->Get(isolate);
    if (promise->CreationContext() == safe && deferred->CreationContext() == safe){ return Undefined(isolate); }

    bool hasCallableResolve = resolve->IsFunction();
    if (hasCallableResolve && Local<Function>::Cast(resolve)->CreationContext() == safe){ return Undefined(isolate); }

    bool hasCallableReject = reject->IsFunction();
    if (hasCallableReject && Local<Function>::Cast(reject)->CreationContext() == safe){ return Undefined(isolate); }

    if (hasCallableResolve || hasCallableReject){
        Local<Value> argv[4] = { promise, resolve, reject, deferred };
        return monitorInstrumentation->Get(isolate)->Call(Undefined(isolate), 4, argv);
    }
    else {
        return Undefined(isolate);
    }

}

