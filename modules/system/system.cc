
#include <sys/resource.h>
#include <unistd.h>
#include <spin.h>

namespace spin {
namespace system {

void clock_gettimeSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  timespec* v1 = reinterpret_cast<timespec*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  int32_t rc = clock_gettime(v0, v1);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t clock_gettimeFast(void* p, int32_t p0, uint64_t p1) {
  int32_t v0 = p0;
  timespec* v1 = reinterpret_cast<timespec*>(p1);
  return clock_gettime(v0, v1);
}

void getpidSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();

  int32_t rc = getpid();
  args.GetReturnValue().Set(Integer::New(isolate, rc));
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
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t getrusageFast(void* p, int32_t p0, uint64_t p1) {
  int32_t v0 = p0;
  struct rusage* v1 = reinterpret_cast<struct rusage*>(p1);
  return getrusage(v0, v1);
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
  SET_MODULE(isolate, target, "system", module);
}
} // namespace system
} // namespace spin

extern "C" {
  void* _register_system() {
    return (void*)spin::system::Init;
  }
}


