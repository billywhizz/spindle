#include <sys/resource.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/timerfd.h>
#include <spin.h>

namespace spin {
namespace system {

void clock_gettimeSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  timespec* v1 = reinterpret_cast<timespec*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  int32_t rc = clock_gettime(v0, v1);
  args.GetReturnValue().Set(Number::New(isolate, rc));
}

int32_t clock_gettimeFast(void* p, int32_t p0, void* p1) {
  int32_t v0 = p0;
  timespec* v1 = reinterpret_cast<timespec*>(p1);
  return clock_gettime(v0, v1);
}

void getpidSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int32_t rc = getpid();
  args.GetReturnValue().Set(Number::New(isolate, rc));
}

int32_t getpidFast(void* p) {

  return getpid();
}

void getrusageSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  struct rusage* v1 = reinterpret_cast<struct rusage*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  int32_t rc = getrusage(v0, v1);
  args.GetReturnValue().Set(Number::New(isolate, rc));
}

int32_t getrusageFast(void* p, int32_t p0, void* p1) {
  int32_t v0 = p0;
  struct rusage* v1 = reinterpret_cast<struct rusage*>(p1);
  return getrusage(v0, v1);
}

void dlopenSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  const char* v0 = reinterpret_cast<const char*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  int32_t v1 = Local<Integer>::Cast(args[1])->Value();
  void* rc = dlopen(v0, v1);
  args.GetReturnValue().Set(Number::New(isolate, reinterpret_cast<uint64_t>(rc)));
}

void dlopenFast(void* p, void* p0, int32_t p1, struct FastApiTypedArray* const p_ret) {
  const char* v0 = reinterpret_cast<const char*>(p0);
  int32_t v1 = p1;
  void* r = dlopen(v0, v1);
  ((void**)p_ret->data)[0] = r;

}

void dlsymSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  void* v0 = reinterpret_cast<void*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  const char* v1 = reinterpret_cast<const char*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  void* rc = dlsym(v0, v1);
  args.GetReturnValue().Set(Number::New(isolate, reinterpret_cast<uint64_t>(rc)));
}

void dlsymFast(void* p, void* p0, void* p1, struct FastApiTypedArray* const p_ret) {
  void* v0 = reinterpret_cast<void*>(p0);
  const char* v1 = reinterpret_cast<const char*>(p1);
  void* r = dlsym(v0, v1);
  ((void**)p_ret->data)[0] = r;

}

void dlcloseSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  void* v0 = reinterpret_cast<void*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  int32_t rc = dlclose(v0);
  args.GetReturnValue().Set(Number::New(isolate, rc));
}

int32_t dlcloseFast(void* p, void* p0) {
  void* v0 = reinterpret_cast<void*>(p0);
  return dlclose(v0);
}

void timerfd_createSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  int32_t v1 = Local<Integer>::Cast(args[1])->Value();
  int32_t rc = timerfd_create(v0, v1);
  args.GetReturnValue().Set(Number::New(isolate, rc));
}

int32_t timerfd_createFast(void* p, int32_t p0, int32_t p1) {
  int32_t v0 = p0;
  int32_t v1 = p1;
  return timerfd_create(v0, v1);
}

void timerfd_settimeSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  int32_t v1 = Local<Integer>::Cast(args[1])->Value();
  const struct itimerspec* v2 = reinterpret_cast<const struct itimerspec*>((uint64_t)args[2]->NumberValue(context).ToChecked());
  struct itimerspec* v3 = reinterpret_cast<struct itimerspec*>((uint64_t)args[3]->NumberValue(context).ToChecked());
  int32_t rc = timerfd_settime(v0, v1, v2, v3);
  args.GetReturnValue().Set(Number::New(isolate, rc));
}

int32_t timerfd_settimeFast(void* p, int32_t p0, int32_t p1, void* p2, void* p3) {
  int32_t v0 = p0;
  int32_t v1 = p1;
  const struct itimerspec* v2 = reinterpret_cast<const struct itimerspec*>(p2);
  struct itimerspec* v3 = reinterpret_cast<struct itimerspec*>(p3);
  return timerfd_settime(v0, v1, v2, v3);
}

void Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);

  v8::CTypeInfo* cargsclock_gettime = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsclock_gettime[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsclock_gettime[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargsclock_gettime[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  v8::CTypeInfo* rcclock_gettime = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infoclock_gettime = new v8::CFunctionInfo(*rcclock_gettime, 3, cargsclock_gettime);
  v8::CFunction* pFclock_gettime = new v8::CFunction((const void*)&clock_gettimeFast, infoclock_gettime);
  SET_FAST_METHOD(isolate, module, "clock_gettime", pFclock_gettime, clock_gettimeSlow);

  v8::CTypeInfo* cargsgetpid = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsgetpid[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);

  v8::CTypeInfo* rcgetpid = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infogetpid = new v8::CFunctionInfo(*rcgetpid, 1, cargsgetpid);
  v8::CFunction* pFgetpid = new v8::CFunction((const void*)&getpidFast, infogetpid);
  SET_FAST_METHOD(isolate, module, "getpid", pFgetpid, getpidSlow);

  v8::CTypeInfo* cargsgetrusage = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsgetrusage[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsgetrusage[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargsgetrusage[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  v8::CTypeInfo* rcgetrusage = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infogetrusage = new v8::CFunctionInfo(*rcgetrusage, 3, cargsgetrusage);
  v8::CFunction* pFgetrusage = new v8::CFunction((const void*)&getrusageFast, infogetrusage);
  SET_FAST_METHOD(isolate, module, "getrusage", pFgetrusage, getrusageSlow);

  v8::CTypeInfo* cargsdlopen = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsdlopen[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsdlopen[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsdlopen[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcdlopen = new v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  v8::CFunctionInfo* infodlopen = new v8::CFunctionInfo(*rcdlopen, 3, cargsdlopen);
  v8::CFunction* pFdlopen = new v8::CFunction((const void*)&dlopenFast, infodlopen);
  SET_FAST_METHOD(isolate, module, "dlopen", pFdlopen, dlopenSlow);

  v8::CTypeInfo* cargsdlsym = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsdlsym[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsdlsym[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsdlsym[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  v8::CTypeInfo* rcdlsym = new v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  v8::CFunctionInfo* infodlsym = new v8::CFunctionInfo(*rcdlsym, 3, cargsdlsym);
  v8::CFunction* pFdlsym = new v8::CFunction((const void*)&dlsymFast, infodlsym);
  SET_FAST_METHOD(isolate, module, "dlsym", pFdlsym, dlsymSlow);

  v8::CTypeInfo* cargsdlclose = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsdlclose[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsdlclose[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  v8::CTypeInfo* rcdlclose = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infodlclose = new v8::CFunctionInfo(*rcdlclose, 2, cargsdlclose);
  v8::CFunction* pFdlclose = new v8::CFunction((const void*)&dlcloseFast, infodlclose);
  SET_FAST_METHOD(isolate, module, "dlclose", pFdlclose, dlcloseSlow);

  v8::CTypeInfo* cargstimerfd_create = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargstimerfd_create[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargstimerfd_create[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargstimerfd_create[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rctimerfd_create = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infotimerfd_create = new v8::CFunctionInfo(*rctimerfd_create, 3, cargstimerfd_create);
  v8::CFunction* pFtimerfd_create = new v8::CFunction((const void*)&timerfd_createFast, infotimerfd_create);
  SET_FAST_METHOD(isolate, module, "timerfd_create", pFtimerfd_create, timerfd_createSlow);

  v8::CTypeInfo* cargstimerfd_settime = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargstimerfd_settime[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargstimerfd_settime[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargstimerfd_settime[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargstimerfd_settime[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargstimerfd_settime[4] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  v8::CTypeInfo* rctimerfd_settime = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infotimerfd_settime = new v8::CFunctionInfo(*rctimerfd_settime, 5, cargstimerfd_settime);
  v8::CFunction* pFtimerfd_settime = new v8::CFunction((const void*)&timerfd_settimeFast, infotimerfd_settime);
  SET_FAST_METHOD(isolate, module, "timerfd_settime", pFtimerfd_settime, timerfd_settimeSlow);
  SET_MODULE(isolate, target, "system", module);
}
} // namespace system
} // namespace spin

extern "C" {
  void* _register_system() {
    return (void*)spin::system::Init;
  }
}
