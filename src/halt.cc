#include "halt.h"

using namespace v8;

void Start(const FunctionCallbackInfo<Value>& args){

	Isolate* isolate = args.GetIsolate();
	if (args.Length() != 1) { isolate->ThrowException(Exception::RangeError(String::NewFromUtf8(isolate, "Wrong number of arguments"))); return; }
	if (!args[0]->IsNumber()) { isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "A number is expected"))); return; }

	uint64_t timeoutValue = args[0]->IntegerValue();
	watcher->Start(timeoutValue);

}
void Clear(const FunctionCallbackInfo<Value>& args){

	Isolate* isolate = args.GetIsolate();
	watcher->Clear();
}
void Protect(const FunctionCallbackInfo<Value>& args) {

	Isolate* isolate = args.GetIsolate();
	if (args.Length() != 1) { isolate->ThrowException(Exception::RangeError(String::NewFromUtf8(isolate, "Wrong number of arguments"))); return; }
	if (!args[0]->IsFunction()) { isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "A function is expected"))); return; }
	
	Local<Function> fn = Local<Function>::Cast(args[0]);
	TryCatch trycatch;
	Local<Value> result = fn->Call(isolate->GetCurrentContext()->Global(), 0, NULL);

	if (result.IsEmpty()){
		if (trycatch.HasCaught() && trycatch.HasTerminated()){
			V8::CancelTerminateExecution(isolate);
			isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Terminated, CPU bound interruption")));
		}
		trycatch.ReThrow();
	}
	else {
		if (V8::IsExecutionTerminating(isolate)){ V8::CancelTerminateExecution(isolate); }
		args.GetReturnValue().Set(result);
	}

}
void Run(const FunctionCallbackInfo<Value>& args) {

	Isolate* isolate = args.GetIsolate();
	if (args.Length() != 2) { isolate->ThrowException(Exception::RangeError(String::NewFromUtf8(isolate, "Wrong number of arguments"))); return; }
	if (!args[0]->IsFunction()) { isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "A function is expected"))); return; }
	if (!args[1]->IsNumber()) { isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "A numeric value is expected"))); return; }

	Local<Function> fn = Local<Function>::Cast(args[0]);
	uint64_t timeoutValue = args[1]->IntegerValue();

	TryCatch trycatch;
	watcher->Start(timeoutValue);
	Local<Value> result = fn->Call(isolate->GetCurrentContext()->Global(), 0, NULL);
	Watcher::STATUS status = watcher->Clear();

	if (result.IsEmpty()){
		if (trycatch.HasCaught() && trycatch.HasTerminated()){
			V8::CancelTerminateExecution(isolate);
			isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Terminated, CPU bound interruption")));
		}
		trycatch.ReThrow();
	}
	else {
		if (status == Watcher::STATUS::TERMINATED || V8::IsExecutionTerminating(isolate)){ V8::CancelTerminateExecution(isolate); }
		args.GetReturnValue().Set(result);
	}

}
void Init(Handle<Object> exports) {

	watcher = new Watcher(Isolate::GetCurrent());

	NODE_SET_METHOD(exports, "start", Start);
	NODE_SET_METHOD(exports, "clear", Clear);
	NODE_SET_METHOD(exports, "protect", Protect);
	NODE_SET_METHOD(exports, "run", Run);

}

NODE_MODULE(addon, Init)