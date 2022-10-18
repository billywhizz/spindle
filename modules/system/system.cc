extern "C" {

}
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

void Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);

  v8::CTypeInfo* cargsclock_gettime = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsclock_gettime[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsclock_gettime[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargsclock_gettime[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  v8::CTypeInfo* rcclock_gettime = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infoclock_gettime = new v8::CFunctionInfo(*rcclock_gettime, 3, cargsclock_gettime);
  v8::CFunction* pFclock_gettime = new v8::CFunction((const void*)&clock_gettimeFast, infoclock_gettime);
  SET_FAST_METHOD2(isolate, module, "clock_gettime", pFclock_gettime, clock_gettimeSlow);
  SET_MODULE(isolate, target, "system", module);
}
} // namespace system
} // namespace spin

extern "C" {
  void* _register_system() {
    return (void*)spin::system::Init;
  }
}


