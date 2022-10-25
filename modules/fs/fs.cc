#include <unistd.h>
#include <spin.h>

namespace spin {
namespace fs {

void closeSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  int32_t rc = close(v0);
  args.GetReturnValue().Set(Number::New(isolate, rc));
}

int32_t closeFast(void* p, int32_t p0) {
  int32_t v0 = p0;
  return close(v0);
}

void openSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  Local<Context> context = isolate->GetCurrentContext();
  const char* v0 = reinterpret_cast<const char*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  int32_t v1 = Local<Integer>::Cast(args[1])->Value();
  int32_t rc = open(v0, v1);
  args.GetReturnValue().Set(Number::New(isolate, rc));
}

int32_t openFast(void* p, void* p0, int32_t p1) {
  const char* v0 = reinterpret_cast<const char*>(p0);
  int32_t v1 = p1;
  return open(v0, v1);
}

void readSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  void* v1 = reinterpret_cast<void*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  int32_t v2 = Local<Integer>::Cast(args[2])->Value();
  int32_t rc = read(v0, v1, v2);
  args.GetReturnValue().Set(Number::New(isolate, rc));
}

int32_t readFast(void* p, int32_t p0, void* p1, int32_t p2) {
  int32_t v0 = p0;
  void* v1 = reinterpret_cast<void*>(p1);
  int32_t v2 = p2;
  return read(v0, v1, v2);
}

void lseekSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  uint32_t v1 = Local<Integer>::Cast(args[1])->Value();
  int32_t v2 = Local<Integer>::Cast(args[2])->Value();
  uint32_t rc = lseek(v0, v1, v2);
  args.GetReturnValue().Set(Number::New(isolate, rc));
}

uint32_t lseekFast(void* p, int32_t p0, uint32_t p1, int32_t p2) {
  int32_t v0 = p0;
  uint32_t v1 = p1;
  int32_t v2 = p2;
  return lseek(v0, v1, v2);
}

void writeSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  void* v1 = reinterpret_cast<void*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  int32_t v2 = Local<Integer>::Cast(args[2])->Value();
  int32_t rc = write(v0, v1, v2);
  args.GetReturnValue().Set(Number::New(isolate, rc));
}

int32_t writeFast(void* p, int32_t p0, void* p1, int32_t p2) {
  int32_t v0 = p0;
  void* v1 = reinterpret_cast<void*>(p1);
  int32_t v2 = p2;
  return write(v0, v1, v2);
}

void Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);

  v8::CTypeInfo* cargsclose = (v8::CTypeInfo*)calloc(2, sizeof(v8::CTypeInfo));
  cargsclose[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsclose[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcclose = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infoclose = new v8::CFunctionInfo(*rcclose, 2, cargsclose);
  v8::CFunction* pFclose = new v8::CFunction((const void*)&closeFast, infoclose);
  SET_FAST_METHOD(isolate, module, "close", pFclose, closeSlow);

  v8::CTypeInfo* cargsopen = (v8::CTypeInfo*)calloc(3, sizeof(v8::CTypeInfo));
  cargsopen[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsopen[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsopen[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcopen = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infoopen = new v8::CFunctionInfo(*rcopen, 3, cargsopen);
  v8::CFunction* pFopen = new v8::CFunction((const void*)&openFast, infoopen);
  SET_FAST_METHOD(isolate, module, "open", pFopen, openSlow);

  v8::CTypeInfo* cargsread = (v8::CTypeInfo*)calloc(4, sizeof(v8::CTypeInfo));
  cargsread[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsread[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargsread[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsread[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcread = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* inforead = new v8::CFunctionInfo(*rcread, 4, cargsread);
  v8::CFunction* pFread = new v8::CFunction((const void*)&readFast, inforead);
  SET_FAST_METHOD(isolate, module, "read", pFread, readSlow);

  v8::CTypeInfo* cargslseek = (v8::CTypeInfo*)calloc(4, sizeof(v8::CTypeInfo));
  cargslseek[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargslseek[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargslseek[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  cargslseek[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rclseek = new v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  v8::CFunctionInfo* infolseek = new v8::CFunctionInfo(*rclseek, 4, cargslseek);
  v8::CFunction* pFlseek = new v8::CFunction((const void*)&lseekFast, infolseek);
  SET_FAST_METHOD(isolate, module, "lseek", pFlseek, lseekSlow);

  v8::CTypeInfo* cargswrite = (v8::CTypeInfo*)calloc(4, sizeof(v8::CTypeInfo));
  cargswrite[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargswrite[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargswrite[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargswrite[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcwrite = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infowrite = new v8::CFunctionInfo(*rcwrite, 4, cargswrite);
  v8::CFunction* pFwrite = new v8::CFunction((const void*)&writeFast, infowrite);
  SET_FAST_METHOD(isolate, module, "write", pFwrite, writeSlow);
  SET_MODULE(isolate, target, "fs", module);
}
} // namespace fs
} // namespace spin

extern "C" {
  void* _register_fs() {
    return (void*)spin::fs::Init;
  }
}
