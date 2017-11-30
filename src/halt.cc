#include "watcher.h"
#include "monitor.h"
#include <node.h>
#include <v8.h>

using namespace v8;


void Start(const FunctionCallbackInfo<Value>& args){
    
    Isolate* isolate = args.GetIsolate();

    if (args.Length() != 1) { 
        isolate->ThrowException(
            Exception::RangeError(
                String::NewFromUtf8(isolate, "Wrong number of arguments")
            )
        ); 
        return; 
    }

    if (!args[0]->IsNumber()) { 
        isolate->ThrowException(
            Exception::TypeError(
                String::NewFromUtf8(isolate, "A number is expected")
            )
        ); 
        return; 
    }

    uint64_t timeout = args[0]->IntegerValue();

    if (timeout < 1) { 
        isolate->ThrowException(
            Exception::RangeError(
                String::NewFromUtf8(isolate, "The timeout parameter must greater than 0")
            )
        ); 
        return; 
    }
    
    Watcher::Start(timeout);

}
void Stop(const FunctionCallbackInfo<Value>& args){

    Watcher::Stop();

}
void Protect(const FunctionCallbackInfo<Value>& args) {

	Isolate* isolate = args.GetIsolate();

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
	
	Local<Function> fn = Local<Function>::Cast(args[0]);

	TryCatch trycatch;
	Local<Value> result = fn->Call(isolate->GetCurrentContext()->Global(), 0, NULL);

	if (result.IsEmpty()){
		if (trycatch.HasCaught() && trycatch.HasTerminated()){
			V8::CancelTerminateExecution(isolate);
			isolate->ThrowException(
                Exception::Error(
                    String::NewFromUtf8(isolate, "Terminated, CPU bound interruption")
                )
            );
		}
		trycatch.ReThrow();
	}
	else {
		if (V8::IsExecutionTerminating(isolate)){ 
            V8::CancelTerminateExecution(isolate); 
        }
		args.GetReturnValue().Set(result);
	}

}
void Run(const FunctionCallbackInfo<Value>& args) {

	Isolate* isolate = args.GetIsolate();

	if (args.Length() != 2) { 
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

	if (!args[1]->IsNumber()) { 
        isolate->ThrowException(
            Exception::TypeError(
                String::NewFromUtf8(isolate, "A numeric value is expected")
            )
        ); 
        return;
    }

	Local<Function> fn = Local<Function>::Cast(args[0]);
	uint64_t timeout = args[1]->IntegerValue();
	
    if (timeout < 1) { 
        isolate->ThrowException(
            Exception::RangeError(
                String::NewFromUtf8(isolate, "The timeout parameter must greater than 0")
            )
        ); 
        return; 
    }

	TryCatch trycatch;
    Watcher::Start(timeout);
	Local<Value> result = fn->Call(isolate->GetCurrentContext()->Global(), 0, NULL);
    Watcher::Stop();

	if (result.IsEmpty()){
		if (trycatch.HasCaught() && trycatch.HasTerminated()){
			V8::CancelTerminateExecution(isolate);
            isolate->ThrowException(
                Exception::Error(
                    String::NewFromUtf8(isolate, "Terminated, CPU bound interruption")
                )
            );
		}
		trycatch.ReThrow();
	}
	else {
        if (V8::IsExecutionTerminating(isolate)){ 
            V8::CancelTerminateExecution(isolate); 
        }
		args.GetReturnValue().Set(result);
	}
}
void Init(Handle<Object> exports) {

    Watcher::Initialize();
    NODE_SET_METHOD(exports, "start", Start);
    NODE_SET_METHOD(exports, "stop", Stop);
    NODE_SET_METHOD(exports, "protect", Protect);
    NODE_SET_METHOD(exports, "run", Run);

    Monitor::Initialize();
    NODE_SET_METHOD(exports, "hook", Monitor::Hook);
    NODE_SET_METHOD(exports, "unhook", Monitor::Unhook);

}


NODE_MODULE(addon, Init);

