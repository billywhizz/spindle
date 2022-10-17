extern "C" {
#include "pico.h"
}
#include <spin.h>

namespace spin {

namespace pico {

void ParseRequest(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int bytes = Local<Integer>::Cast(args[1])->Value();
  char* n = reinterpret_cast<char*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  httpRequest* r = reinterpret_cast<httpRequest*>((uint64_t)args[2]->NumberValue(context).ToChecked());
  int rc = parse_request(n, bytes, r);
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), rc));
}

void ParseResponse(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int bytes = Local<Integer>::Cast(args[1])->Value();
  char* n = reinterpret_cast<char*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  httpResponse* r = reinterpret_cast<httpResponse*>((uint64_t)args[2]->NumberValue(context).ToChecked());
  int rc = parse_response(n, bytes, r);
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), rc));
}

int ParseResponseFast(void* p, uint64_t next, uint32_t bytes, uint64_t res) {
  char* n = reinterpret_cast<char*>(next);
  httpResponse* r = reinterpret_cast<httpResponse*>(res);
  return parse_response(n, bytes, r);
}

int ParseRequestFast(void* p, uint64_t next, uint32_t bytes, uint64_t res) {
  char* n = reinterpret_cast<char*>(next);
  httpRequest* r = reinterpret_cast<httpRequest*>(res);
  return parse_request(n, bytes, r);
}

void Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  v8::CTypeInfo* cargs = (v8::CTypeInfo*)calloc(4, sizeof(v8::CTypeInfo));
  cargs[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargs[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargs[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  cargs[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  v8::CTypeInfo* rc = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* info = new v8::CFunctionInfo(*rc, 4, cargs);
  v8::CFunction* parseRequestFast = new v8::CFunction((const void*)&ParseRequestFast, info);
  v8::CFunction* parseResponseFast = new v8::CFunction((const void*)&ParseResponseFast, info);
  SET_FAST_METHOD2(isolate, module, "parseRequest", parseRequestFast, ParseRequest);
  SET_FAST_METHOD2(isolate, module, "parseResponse", parseResponseFast, ParseResponse);
  SET_MODULE(isolate, target, "pico", module);
}

}

}

extern "C" {
	void* _register_pico() {
		return (void*)spin::pico::Init;
	}
}
