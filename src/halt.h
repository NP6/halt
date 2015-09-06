#ifndef HALT_H
#define HALT_H

	#include "Watcher.h"
	#include <node.h>
	#include <v8.h>

	Watcher* watcher;

	void Start(const FunctionCallbackInfo<Value>& args);
	void Clear(const FunctionCallbackInfo<Value>& args);
	void Protect(const FunctionCallbackInfo<Value>& args);
	void Run(const FunctionCallbackInfo<Value>& args);
	void Init(Handle<Object> exports);

#endif