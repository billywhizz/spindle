#pragma once

#include <v8.h>
#include <libplatform/libplatform.h>
#include <map>
#include <unistd.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <v8-fast-api-calls.h>
#include <dlfcn.h>

namespace spin {

#define MICROS_PER_SEC 1e6

using v8::String;
using v8::NewStringType;
using v8::Local;
using v8::Isolate;
using v8::Context;
using v8::ObjectTemplate;
using v8::FunctionCallbackInfo;
using v8::Function;
using v8::Object;
using v8::Value;
using v8::MaybeLocal;
using v8::Module;
using v8::TryCatch;
using v8::Message;
using v8::StackTrace;
using v8::StackFrame;
using v8::HandleScope;
using v8::Integer;
using v8::BigInt;
using v8::FunctionTemplate;
using v8::ScriptOrigin;
using v8::True;
using v8::False;
using v8::ScriptCompiler;
using v8::ArrayBuffer;
using v8::Array;
using v8::Maybe;
using v8::HeapStatistics;
using v8::Float64Array;
using v8::HeapSpaceStatistics;
using v8::BigUint64Array;
using v8::Int32Array;
using v8::Exception;
using v8::FunctionCallback;
using v8::Script;
using v8::Platform;
using v8::V8;
using v8::BackingStore;
using v8::SharedArrayBuffer;
using v8::PromiseRejectMessage;
using v8::Promise;
using v8::PromiseRejectEvent;
using v8::Uint32Array;
using v8::BigUint64Array;
using v8::FixedArray;
using v8::Number;

struct rawBuffer {
  uint32_t len;
  int32_t read;
  int32_t written;
  uint32_t id;
  uint8_t* data;
};

struct builtin {
  unsigned int size;
  const char* source;
};

enum ScriptType : int {
  kScript,
  kModule,
  kFunction,
};

enum HostDefinedOptions : int {
  kType = 8,
  kID = 9,
  kLength = 10,
};

typedef void *(*register_plugin)();
extern std::map<std::string, builtin*> builtins;
extern std::map<std::string, register_plugin> modules;

ssize_t process_memory_usage();
uint64_t hrtime();
void builtins_add (const char* name, const char* source, 
  unsigned int size);

using InitializerCallback = void (*)(Isolate* isolate, 
  Local<ObjectTemplate> exports);
v8::MaybeLocal<v8::Module> OnModuleInstantiate(v8::Local<v8::Context> context,
  v8::Local<v8::String> specifier, v8::Local<v8::FixedArray> import_assertions, 
  v8::Local<v8::Module> referrer);

int CreateIsolate(int argc, char** argv, 
  const char* main, unsigned int main_len,
  const char* js, unsigned int js_len, struct iovec* buf, int fd,
  uint64_t start, const char* globalobj, const char* scriptname);
int CreateIsolate(int argc, char** argv,
  const char* main, unsigned int main_len, uint64_t start,
  const char* globalobj);
void PrintStackTrace(Isolate* isolate, const TryCatch& try_catch);
void PromiseRejectCallback(PromiseRejectMessage message);
void FreeMemory(void* buf, size_t length, void* data);

void SET_METHOD(Isolate *isolate, Local<ObjectTemplate> 
  recv, const char *name, FunctionCallback callback);
void SET_MODULE(Isolate *isolate, Local<ObjectTemplate> 
  recv, const char *name, Local<ObjectTemplate> module);
void SET_VALUE(Isolate *isolate, Local<ObjectTemplate> 
  recv, const char *name, Local<Value> value);

template <typename F>
void SET_FAST_METHOD(Isolate* isolate, v8::Local<v8::ObjectTemplate> exports, const char * name, F* fastFunc, v8::FunctionCallback slowFunc) {
  v8::CFunction fastCFunc = v8::CFunction::Make(fastFunc);
  v8::Local<v8::FunctionTemplate> funcTemplate = v8::FunctionTemplate::New(
    isolate,
    slowFunc,
    v8::Local<v8::Value>(),
    v8::Local<v8::Signature>(),
    0,
    v8::ConstructorBehavior::kThrow,
    v8::SideEffectType::kHasSideEffect,
    &fastCFunc
  );
  exports->Set(
    v8::String::NewFromUtf8(isolate, name).ToLocalChecked(),
    funcTemplate
  );
}

template <typename F>
void SET_FAST_METHOD2(Isolate* isolate, v8::Local<v8::ObjectTemplate> exports, const char * name, F* fastCFunc, v8::FunctionCallback slowFunc) {
  v8::Local<v8::FunctionTemplate> funcTemplate = v8::FunctionTemplate::New(
    isolate,
    slowFunc,
    v8::Local<v8::Value>(),
    v8::Local<v8::Signature>(),
    0,
    v8::ConstructorBehavior::kThrow,
    v8::SideEffectType::kHasSideEffect,
    fastCFunc
  );
  exports->Set(
    v8::String::NewFromUtf8(isolate, name).ToLocalChecked(),
    funcTemplate
  );
}

void MemoryUsage(const FunctionCallbackInfo<Value> &args);
void Load(const FunctionCallbackInfo<Value> &args);
void Builtin(const FunctionCallbackInfo<Value> &args);
void Builtins(const FunctionCallbackInfo<Value> &args);
void Modules(const FunctionCallbackInfo<Value> &args);
void NextTick(const FunctionCallbackInfo<Value> &args);
void Compile(const FunctionCallbackInfo<Value> &args);
void RunScript(const FunctionCallbackInfo<Value> &args);

void ReadFile(const FunctionCallbackInfo<Value> &args);
void Print(const FunctionCallbackInfo<Value> &args);
void Error(const FunctionCallbackInfo<Value> &args);
void Sleep(const FunctionCallbackInfo<Value> &args);
void Calloc(const FunctionCallbackInfo<Value> &args);

void DLOpen(const FunctionCallbackInfo<Value> &args);
void DLSym(const FunctionCallbackInfo<Value> &args);
void DLClose(const FunctionCallbackInfo<Value> &args);
void DLError(const FunctionCallbackInfo<Value> &args);

void GetAddress(const FunctionCallbackInfo<Value> &args);
void ReadMemory(const FunctionCallbackInfo<Value> &args);
void ReadString(const FunctionCallbackInfo<Value> &args);
void Utf8Length(const FunctionCallbackInfo<Value> &args);

void WriteUtf16(const FunctionCallbackInfo<Value> &args);
void WriteUtf8(const FunctionCallbackInfo<Value> &args);
void WriteLatin1(const FunctionCallbackInfo<Value> &args);

void ReadUtf16(const FunctionCallbackInfo<Value> &args);
void ReadUtf8(const FunctionCallbackInfo<Value> &args);
void ReadLatin1(const FunctionCallbackInfo<Value> &args);

void RawBuffer(const FunctionCallbackInfo<Value> &args);

void PID(const FunctionCallbackInfo<Value> &args);
void HRTime(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);

}
