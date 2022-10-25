#include <spin.h>

namespace spin {
namespace test {

Persistent<Function, v8::NonCopyablePersistentTraits<Function>> onEvent;
Persistent<Int32Array, v8::NonCopyablePersistentTraits<Function>> eventIds;
Persistent<Array, v8::NonCopyablePersistentTraits<Array>> eventObjects;
int32_t *fields;
int32_t size;

void Register(const FunctionCallbackInfo<Value> &args) {
  onEvent.Reset(args.GetIsolate(), args[0].As<Function>());
  Local<Int32Array> i32 = args[1].As<Int32Array>();
  Local<Array> objs = args[2].As<Array>();
  size = i32->Length();
  fields = static_cast<int32_t *>(i32->Buffer()->Data());
  eventIds.Reset(args.GetIsolate(), i32);
  eventObjects.Reset(args.GetIsolate(), objs);
}

/*
This is roughly equivalent to current method in Deno, which does work
asynchronously and sends op ids and related result objects back to JS by 
building a set of JS arguments as pairs of [opid, result], e.g.
[id1, result1, id2, result2....]
and passes this list of arguments to a single callback into JS at the end
*/
void Poll1(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Function> handler = Local<Function>::New(isolate, onEvent);
  Local<Value> argv[size * 2] = {};
  Local<v8::Primitive> undefined = v8::Undefined(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> obj = v8::Object::New(isolate);
  int j = 0;
  for (int i = 0; i < size; i++) {
    argv[j++] = v8::Integer::NewFromUnsigned(isolate, i);
    argv[j++] = obj;
  }
  handler->Call(context, undefined, size * 2, argv).ToLocalChecked();
}

/*
This is roughly equivalent to proposed method of calling each Promise resolve
function from Rust individually rather than building an array of results to
send back in a single call.
*/
void Poll2(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Function> handler = Local<Function>::New(isolate, onEvent);
  Local<Value> argv[1] = {};
  Local<v8::Primitive> undefined = v8::Undefined(isolate);
  Local<Object> obj = v8::Object::New(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  for (int i = 0; i < size; i++) {
    argv[0] = obj;
    handler->Call(context, undefined, 1, argv).ToLocalChecked();
  }
}

/*
This is the same as the first, existing, method of calling back into JS once, 
but this updates an Int32Array shared between JS and Rust with the opids
instead of passing a JS Integer as an argument. This means only half the number
of arguments need to be passed to the JS function call and less young 
generation garbage created as we don't need to create the JS Integers as args.
*/
void Poll3(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Function> handler = Local<Function>::New(isolate, onEvent);
  Local<Value> argv[size] = {};
  Local<v8::Primitive> undefined = v8::Undefined(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> obj = v8::Object::New(isolate);
  for (int i = 0; i < size; i++) {
    fields[i] = i;
    argv[i] = obj;
  }
  handler->Call(context, undefined, size, argv).ToLocalChecked();
}

void Poll4(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Function> handler = Local<Function>::New(isolate, onEvent);
  Local<v8::Primitive> undefined = v8::Undefined(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Array> objs = Local<Array>::New(isolate, eventObjects);
  Local<Object> obj = v8::Object::New(isolate);
  for (int i = 0; i < size; i++) {
    fields[i] = i;
    objs->Set(context, i, obj).Check();
  }
  Local<Value> argv[1] = { Integer::New(isolate, size) };
  handler->Call(context, undefined, 1, argv).ToLocalChecked();
}

void Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "register", Register);

  SET_METHOD(isolate, module, "poll1", Poll1);
  SET_METHOD(isolate, module, "poll2", Poll2);
  SET_METHOD(isolate, module, "poll3", Poll3);
  SET_METHOD(isolate, module, "poll4", Poll4);

  SET_MODULE(isolate, target, "test", module);
}

} // namespace test
} // namespace spin

extern "C" {
  void* _register_test() {
    return (void*)spin::test::Init;
  }
}
