#include "spin.h"

std::map<std::string, spin::builtin*> spin::builtins;
std::map<std::string, spin::register_plugin> spin::modules;
uint32_t scriptId = 1;
clock_t clock_id = CLOCK_MONOTONIC;
std::map<int, spin::rawBuffer*> buffers;
int bcount = 0;
struct timespec* hrtimeptr;

// todo: do this in js and keep the fd open
ssize_t spin::process_memory_usage() {
  char buf[1024];
  const char* s = NULL;
  ssize_t n = 0;
  unsigned long val = 0;
  int fd = 0;
  int i = 0;
  do {
    fd = open("/proc/thread-self/stat", O_RDONLY);
  } while (fd == -1 && errno == EINTR);
  if (fd == -1) return (ssize_t)errno;
  do
    n = read(fd, buf, sizeof(buf) - 1);
  while (n == -1 && errno == EINTR);
  close(fd);
  if (n == -1)
    return (ssize_t)errno;
  buf[n] = '\0';
  s = strchr(buf, ' ');
  if (s == NULL)
    goto err;
  s += 1;
  if (*s != '(')
    goto err;
  s = strchr(s, ')');
  if (s == NULL)
    goto err;
  for (i = 1; i <= 22; i++) {
    s = strchr(s + 1, ' ');
    if (s == NULL)
      goto err;
  }
  errno = 0;
  val = strtoul(s, NULL, 10);
  if (errno != 0)
    goto err;
  return val * (unsigned long)getpagesize();
err:
  return 0;
}

uint64_t spin::hrtime() {
  struct timespec t;
  if (clock_gettime(clock_id, &t)) return 0;
  return (t.tv_sec * (uint64_t) 1e9) + t.tv_nsec;
}

void spin::builtins_add (const char* name, const char* source, 
  unsigned int size) {
  struct builtin* b = new builtin();
  b->size = size;
  b->source = source;
  builtins[name] = b;
}

void spin::SET_METHOD(Isolate *isolate, Local<ObjectTemplate> 
  recv, const char *name, FunctionCallback callback) {
  recv->Set(String::NewFromUtf8(isolate, name, 
    NewStringType::kInternalized).ToLocalChecked(), 
    FunctionTemplate::New(isolate, callback));
}

void spin::SET_MODULE(Isolate *isolate, Local<ObjectTemplate> 
  recv, const char *name, Local<ObjectTemplate> module) {
  recv->Set(String::NewFromUtf8(isolate, name, 
    NewStringType::kInternalized).ToLocalChecked(), 
    module);
}

void spin::SET_VALUE(Isolate *isolate, Local<ObjectTemplate> 
  recv, const char *name, Local<Value> value) {
  recv->Set(String::NewFromUtf8(isolate, name, 
    NewStringType::kInternalized).ToLocalChecked(), 
    value);
}

void spin::PrintStackTrace(Isolate* isolate, const TryCatch& try_catch) {
  Local<Message> message = try_catch.Message();
  Local<StackTrace> stack = message->GetStackTrace();
  Local<Value> scriptName = message->GetScriptResourceName();
  String::Utf8Value scriptname(isolate, scriptName);
  Local<Context> context = isolate->GetCurrentContext();
  int linenum = message->GetLineNumber(context).FromJust();
  v8::String::Utf8Value err_message(isolate, message->Get().As<String>());
  fprintf(stderr, "%s in %s on line %i\n", *err_message, *scriptname, linenum);
  if (stack.IsEmpty()) return;
  for (int i = 0; i < stack->GetFrameCount(); i++) {
    Local<StackFrame> stack_frame = stack->GetFrame(isolate, i);
    Local<String> functionName = stack_frame->GetFunctionName();
    Local<String> scriptName = stack_frame->GetScriptName();
    String::Utf8Value fn_name_s(isolate, functionName);
    String::Utf8Value script_name(isolate, scriptName);
    const int line_number = stack_frame->GetLineNumber();
    const int column = stack_frame->GetColumn();
    if (stack_frame->IsEval()) {
      if (stack_frame->GetScriptId() == Message::kNoScriptIdInfo) {
        fprintf(stderr, "    at [eval]:%i:%i\n", line_number, column);
      } else {
        fprintf(stderr, "    at [eval] (%s:%i:%i)\n", *script_name,
          line_number, column);
      }
      break;
    }
    if (fn_name_s.length() == 0) {
      fprintf(stderr, "    at %s:%i:%i\n", *script_name, line_number, column);
    } else {
      fprintf(stderr, "    at %s (%s:%i:%i)\n", *fn_name_s, *script_name,
        line_number, column);
    }
  }
  fflush(stderr);
}

void spin::PromiseRejectCallback(PromiseRejectMessage data) {
  if (data.GetEvent() == v8::kPromiseRejectAfterResolved ||
      data.GetEvent() == v8::kPromiseResolveAfterResolved) {
    return;
  }
  Local<Promise> promise = data.GetPromise();
  Isolate* isolate = promise->GetIsolate();
  if (data.GetEvent() == v8::kPromiseHandlerAddedAfterReject) {
    return;
  }
  Local<Value> exception = data.GetValue();
  v8::Local<Message> message;
  if (exception->IsObject()) {
    message = v8::Exception::CreateMessage(isolate, exception);
  }
  if (!exception->IsNativeError() &&
      (message.IsEmpty() || message->GetStackTrace().IsEmpty())) {
    exception = v8::Exception::Error(
        v8::String::NewFromUtf8Literal(isolate, "Unhandled Promise."));
    message = Exception::CreateMessage(isolate, exception);
  }
  Local<Context> context = isolate->GetCurrentContext();
  TryCatch try_catch(isolate);
  Local<Object> globalInstance = context->Global();
  Local<Value> func = globalInstance->Get(context, 
    String::NewFromUtf8Literal(isolate, "onUnhandledRejection", 
      NewStringType::kNormal)).ToLocalChecked();
  if (func.IsEmpty()) {
    return;
  }
  Local<Function> onUnhandledRejection = Local<Function>::Cast(func);
  if (try_catch.HasCaught()) {
    fprintf(stderr, "PromiseRejectCallback: Cast\n");
    return;
  }
  Local<Value> argv[1] = { exception };
  MaybeLocal<Value> result = onUnhandledRejection->Call(context, 
    globalInstance, 1, argv);
  if (result.IsEmpty() && try_catch.HasCaught()) {
    fprintf(stderr, "PromiseRejectCallback: Call\n");
  }
}

void spin::FreeMemory(void* buf, size_t length, void* data) {
  free(buf);
}

char* readFile(char filename[]) {
  char* contents;
  std::ifstream file;
  file.open(filename, std::ifstream::ate);
  if (!file) {
    spin::builtin* b = spin::builtins[filename];
    if (b != nullptr) {
      char* contents = (char*)calloc(1, b->size);
      memcpy(contents, b->source, b->size);
      return contents;
    }
    contents = new char[1];
    return contents;
  }
  size_t file_size = file.tellg();
  file.seekg(0);
  std::filebuf* file_buf = file.rdbuf();
  contents = new char[file_size + 1]();
  file_buf->sgetn(contents, file_size);
  file.close();
  return contents;
}

v8::MaybeLocal<v8::Module> loadModule(char code[],
                                      char name[],
                                      v8::Local<v8::Context> cx) {
  v8::Local<v8::String> vcode =
      v8::String::NewFromUtf8(cx->GetIsolate(), code).ToLocalChecked();
  v8::Local<v8::PrimitiveArray> opts =
      v8::PrimitiveArray::New(cx->GetIsolate(), spin::HostDefinedOptions::kLength);
  opts->Set(cx->GetIsolate(), spin::HostDefinedOptions::kType,
                            v8::Number::New(cx->GetIsolate(), spin::ScriptType::kModule));
  v8::ScriptOrigin origin(cx->GetIsolate(), v8::String::NewFromUtf8(cx->GetIsolate(), name).ToLocalChecked(), // resource name
    0, // line offset
    0,  // column offset
    true, // is shared cross-origin
    -1,  // script id
    v8::Local<v8::Value>(), // source map url
    false, // is opaque
    false, // is wasm
    true, // is module
    opts);
  v8::Context::Scope context_scope(cx);
  v8::ScriptCompiler::Source source(vcode, origin);
  v8::MaybeLocal<v8::Module> mod;
  mod = v8::ScriptCompiler::CompileModule(cx->GetIsolate(), &source);
  return mod;
}

v8::MaybeLocal<v8::Module> spin::OnModuleInstantiate(v8::Local<v8::Context> context,
  v8::Local<v8::String> specifier,
  v8::Local<v8::FixedArray> import_assertions, 
  v8::Local<v8::Module> referrer) {
  v8::String::Utf8Value str(context->GetIsolate(), specifier);
  return loadModule(readFile(*str), *str, context);
}

v8::Local<v8::Module> checkModule(v8::MaybeLocal<v8::Module> maybeModule,
  v8::Local<v8::Context> cx) {
  v8::Local<v8::Module> mod;
  if (!maybeModule.ToLocal(&mod)) {
    printf("Error loading module!\n");
    exit(EXIT_FAILURE);
  }
  v8::Maybe<bool> result = mod->InstantiateModule(cx, spin::OnModuleInstantiate);
  if (result.IsNothing()) {
    printf("\nCan't instantiate module.\n");
    exit(EXIT_FAILURE);
  }
  return mod;
}

v8::Local<v8::Value> execModule(v8::Local<v8::Module> mod,
  v8::Local<v8::Context> cx,
  bool nsObject) {
  v8::Local<v8::Value> retValue;
  if (!mod->Evaluate(cx).ToLocal(&retValue)) {
    printf("Error evaluating module!\n");
    exit(EXIT_FAILURE);
  }
  if (nsObject)
    return mod->GetModuleNamespace();
  else
    return retValue;
}

v8::MaybeLocal<v8::Promise> OnDynamicImport(v8::Local<v8::Context> context,
  v8::Local<v8::Data> host_defined_options,
  v8::Local<v8::Value> resource_name,
  v8::Local<v8::String> specifier,
  v8::Local<v8::FixedArray> import_assertions) {
  v8::Local<v8::Promise::Resolver> resolver =
      v8::Promise::Resolver::New(context).ToLocalChecked();
  v8::MaybeLocal<v8::Promise> promise(resolver->GetPromise());
  v8::String::Utf8Value name(context->GetIsolate(), specifier);
  v8::Local<v8::Module> mod =
      checkModule(loadModule(readFile(*name), *name, context), context);
  v8::Local<v8::Value> retValue = execModule(mod, context, true);
  resolver->Resolve(context, retValue).ToChecked();
  return promise;
}

void cleanupIsolate (v8::Isolate* isolate) {
  isolate->ContextDisposedNotification();
  isolate->LowMemoryNotification();
  isolate->ClearKeptObjects();
  bool stop = false;
  while(!stop) {
    stop = isolate->IdleNotificationDeadline(1);  
  }
  isolate->Dispose();
}

int spin::CreateIsolate(int argc, char** argv, 
  const char* main_src, unsigned int main_len, 
  const char* js, unsigned int js_len, struct iovec* buf, int fd,
  uint64_t start, const char* globalobj, const char* scriptname) {
  Isolate::CreateParams create_params;
  int statusCode = 0;
  create_params.array_buffer_allocator = 
    ArrayBuffer::Allocator::NewDefaultAllocator();
  create_params.embedder_wrapper_type_index = 0;
  create_params.embedder_wrapper_object_index = 1;
  Isolate *isolate = Isolate::New(create_params);
  {
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    // TODO: make this a config option
    isolate->SetCaptureStackTraceForUncaughtExceptions(true, 1000, 
      StackTrace::kDetailed);
    Local<ObjectTemplate> global = ObjectTemplate::New(isolate);
    Local<ObjectTemplate> runtime = ObjectTemplate::New(isolate);
    spin::Init(isolate, runtime);
    global->Set(String::NewFromUtf8(isolate, globalobj, 
      NewStringType::kInternalized, strnlen(globalobj, 256)).ToLocalChecked(), runtime);
    Local<Context> context = Context::New(isolate, NULL, global);
    Context::Scope context_scope(context);
    isolate->SetPromiseRejectCallback(PromiseRejectCallback);
    isolate->SetHostImportModuleDynamicallyCallback(OnDynamicImport);
    Local<Array> arguments = Array::New(isolate);
    for (int i = 0; i < argc; i++) {
      arguments->Set(context, i, String::NewFromUtf8(isolate, argv[i], 
        NewStringType::kNormal, strlen(argv[i])).ToLocalChecked()).Check();
    }
    Local<Object> globalInstance = context->Global();
    globalInstance->Set(context, String::NewFromUtf8Literal(isolate, 
      "global", 
      NewStringType::kNormal), globalInstance).Check();
    Local<Value> obj = globalInstance->Get(context, 
      String::NewFromUtf8(
        isolate, globalobj, 
        NewStringType::kInternalized, strnlen(globalobj, 256)).ToLocalChecked()).ToLocalChecked();
    Local<Object> runtimeInstance = Local<Object>::Cast(obj);
    if (buf != NULL) {
      std::unique_ptr<BackingStore> backing = SharedArrayBuffer::NewBackingStore(
          buf->iov_base, buf->iov_len, [](void*, size_t, void*){}, nullptr);
      Local<SharedArrayBuffer> ab = SharedArrayBuffer::New(isolate, std::move(backing));
      runtimeInstance->Set(context, String::NewFromUtf8Literal(isolate, 
        "buffer", NewStringType::kNormal), ab).Check();
    }
    if (start > 0) {
      runtimeInstance->Set(context, String::NewFromUtf8Literal(isolate, "start", 
        NewStringType::kNormal), 
        BigInt::New(isolate, start)).Check();
    }
    if (fd != 0) {
      runtimeInstance->Set(context, String::NewFromUtf8Literal(isolate, "fd", 
        NewStringType::kNormal), 
        Integer::New(isolate, fd)).Check();
    }
    runtimeInstance->Set(context, String::NewFromUtf8Literal(isolate, "args", 
      NewStringType::kNormal), arguments).Check();
    if (js_len > 0) {
      runtimeInstance->Set(context, String::NewFromUtf8Literal(isolate, 
        "workerSource", NewStringType::kNormal), 
        String::NewFromUtf8(isolate, js, NewStringType::kNormal, 
        js_len).ToLocalChecked()).Check();
    }
    TryCatch try_catch(isolate);
    Local<v8::PrimitiveArray> opts =
        v8::PrimitiveArray::New(isolate, spin::HostDefinedOptions::kLength);
    opts->Set(isolate, spin::HostDefinedOptions::kType, 
      v8::Number::New(isolate, spin::ScriptType::kModule));
    ScriptOrigin baseorigin(
      isolate,
      String::NewFromUtf8(isolate, scriptname, NewStringType::kInternalized, strnlen(scriptname, 1024)).ToLocalChecked(),
      0, // line offset
      0,  // column offset
      false, // is shared cross-origin
      scriptId++,  // script id
      Local<Value>(), // source map url
      false, // is opaque
      false, // is wasm
      true,  // is module
      opts
    );
    Local<String> base;
    base = String::NewFromUtf8(isolate, main_src, NewStringType::kNormal, 
      main_len).ToLocalChecked();
    ScriptCompiler::Source basescript(base, baseorigin);
    Local<Module> module;
    if (!ScriptCompiler::CompileModule(isolate, &basescript).ToLocal(&module)) {
      PrintStackTrace(isolate, try_catch);
      return 1;
    }
    Maybe<bool> ok2 = module->InstantiateModule(context, spin::OnModuleInstantiate);
    if (ok2.IsNothing()) {
      if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
        try_catch.ReThrow();
      }
      return 1;
    }
    module->Evaluate(context).ToLocalChecked();
    if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
      try_catch.ReThrow();
      return 1;
    }
    Local<Value> func = globalInstance->Get(context, 
      String::NewFromUtf8Literal(isolate, "onExit", 
        NewStringType::kNormal)).ToLocalChecked();
    if (func->IsFunction()) {
      Local<Function> onExit = Local<Function>::Cast(func);
      Local<Value> argv[1] = {Integer::New(isolate, 0)};
      MaybeLocal<Value> result = onExit->Call(context, globalInstance, 0, argv);
      if (!result.IsEmpty()) {
        statusCode = result.ToLocalChecked()->Uint32Value(context).ToChecked();
      }
      if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
        spin::PrintStackTrace(isolate, try_catch);
        return 2;
      }
      statusCode = result.ToLocalChecked()->Uint32Value(context).ToChecked();
    }
  }
  cleanupIsolate(isolate);
  delete create_params.array_buffer_allocator;
  isolate = nullptr;
  return statusCode;
}

int spin::CreateIsolate(int argc, char** argv, const char* main_src, 
  unsigned int main_len, uint64_t start, const char* globalobj) {
  return CreateIsolate(argc, argv, main_src, main_len, NULL, 0, NULL, 0, start, globalobj, "main.js");
}

void spin::Print(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  if (args[0].IsEmpty()) return;
  String::Utf8Value str(args.GetIsolate(), args[0]);
  int endline = 1;
  if (args.Length() > 1) {
    endline = static_cast<int>(args[1]->BooleanValue(isolate));
  }
  const char *cstr = *str;
  if (endline == 1) {
    fprintf(stdout, "%s\n", cstr);
  } else {
    fprintf(stdout, "%s", cstr);
  }
}

void spin::Error(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  if (args[0].IsEmpty()) return;
  String::Utf8Value str(args.GetIsolate(), args[0]);
  int endline = 1;
  if (args.Length() > 1) {
    endline = static_cast<int>(args[1]->BooleanValue(isolate));
  }
  const char *cstr = *str;
  if (endline == 1) {
    fprintf(stderr, "%s\n", cstr);
  } else {
    fprintf(stderr, "%s", cstr);
  }
}

void spin::Load(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<ObjectTemplate> exports = ObjectTemplate::New(isolate);
  if (args[0]->IsString()) {
    String::Utf8Value name(isolate, args[0]);
    auto iter = spin::modules.find(*name);
    if (iter == spin::modules.end()) {
      return;
    } else {
      register_plugin _init = (*iter->second);
      auto _register = reinterpret_cast<InitializerCallback>(_init());
      _register(isolate, exports);
    }
  } else {
    Local<BigInt> address64 = Local<BigInt>::Cast(args[0]);
    void* ptr = reinterpret_cast<void*>(address64->Uint64Value());
    register_plugin _init = reinterpret_cast<register_plugin>(ptr);
    auto _register = reinterpret_cast<InitializerCallback>(_init());
    _register(isolate, exports);
  }
  args.GetReturnValue().Set(exports->NewInstance(context).ToLocalChecked());
}

void spin::Builtin(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  String::Utf8Value name(isolate, args[0]);
  spin::builtin* b = builtins[*name];
  if (b == nullptr) {
    args.GetReturnValue().Set(Null(isolate));
    return;
  }
  if (args.Length() == 1) {
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, b->source, 
      NewStringType::kNormal, b->size).ToLocalChecked());
    return;
  }
  std::unique_ptr<BackingStore> backing = SharedArrayBuffer::NewBackingStore(
      (void*)b->source, b->size, [](void*, size_t, void*){}, nullptr);
  Local<SharedArrayBuffer> ab = SharedArrayBuffer::New(isolate, std::move(backing));
  args.GetReturnValue().Set(ab);
}

void spin::MemoryUsage(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  ssize_t rss = spin::process_memory_usage();
  HeapStatistics v8_heap_stats;
  isolate->GetHeapStatistics(&v8_heap_stats);
  Local<BigUint64Array> array;
  Local<ArrayBuffer> ab;
  if (args.Length() > 0) {
    array = args[0].As<BigUint64Array>();
    ab = array->Buffer();
  } else {
    ab = ArrayBuffer::New(isolate, 16 * 8);
    array = BigUint64Array::New(ab, 0, 16);
  }
  uint64_t *fields = static_cast<uint64_t *>(ab->Data());
  fields[0] = rss;
  fields[1] = v8_heap_stats.total_heap_size();
  fields[2] = v8_heap_stats.used_heap_size();
  fields[3] = v8_heap_stats.external_memory();
  fields[4] = v8_heap_stats.does_zap_garbage();
  fields[5] = v8_heap_stats.heap_size_limit();
  fields[6] = v8_heap_stats.malloced_memory();
  fields[7] = v8_heap_stats.number_of_detached_contexts();
  fields[8] = v8_heap_stats.number_of_native_contexts();
  fields[9] = v8_heap_stats.peak_malloced_memory();
  fields[10] = v8_heap_stats.total_available_size();
  fields[11] = v8_heap_stats.total_heap_size_executable();
  fields[12] = v8_heap_stats.total_physical_size();
  fields[13] = isolate->AdjustAmountOfExternalAllocatedMemory(0);
  args.GetReturnValue().Set(array);
}

void spin::Builtins(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Array> b = Array::New(isolate);
  int i = 0;
  for (auto const& builtin : builtins) {
    b->Set(context, i++, String::NewFromUtf8(isolate, builtin.first.c_str(), 
      NewStringType::kNormal, builtin.first.length()).ToLocalChecked()).Check();
  }
  args.GetReturnValue().Set(b);
}

void spin::Modules(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Array> m = Array::New(isolate);
  int i = 0;
  for (auto const& module : modules) {
    m->Set(context, i++, String::NewFromUtf8(isolate, module.first.c_str(), 
      NewStringType::kNormal, module.first.length()).ToLocalChecked()).Check();
  }
  args.GetReturnValue().Set(m);
}

void spin::NextTick(const FunctionCallbackInfo<Value>& args) {
  args.GetIsolate()->EnqueueMicrotask(args[0].As<Function>());
}

void spin::DLOpen(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int mode = RTLD_LAZY;
  void* handle;
  int argc = args.Length();
  if (argc > 1) mode = Local<Integer>::Cast(args[1])->Value();
  if (argc > 0) {
    String::Utf8Value path(isolate, args[0]);
    handle = dlopen(*path, mode);
  } else {
    handle = dlopen(NULL, mode);
  }
  if (handle == NULL) {
    args.GetReturnValue().Set(v8::Null(isolate));
    return;
  }
  args.GetReturnValue().Set(BigInt::New(isolate, (uint64_t)handle));
}

void spin::DLSym(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<BigInt> address64 = Local<BigInt>::Cast(args[0]);
  String::Utf8Value name(isolate, args[1]);
  void* handle = reinterpret_cast<void*>(address64->Uint64Value());
  void* ptr = dlsym(handle, *name);
  if (ptr == NULL) {
    args.GetReturnValue().Set(v8::Null(isolate));
    return;
  }
  args.GetReturnValue().Set(BigInt::New(isolate, (uint64_t)ptr));
}

void spin::DLError(const FunctionCallbackInfo<Value> &args) {
  char* err = dlerror();
  if (err == NULL) {
    args.GetReturnValue().Set(v8::Null(args.GetIsolate()));
    return;
  }
  args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), err, 
    NewStringType::kNormal).ToLocalChecked());
}

void spin::DLClose(const FunctionCallbackInfo<Value> &args) {
  Local<BigInt> address64 = Local<BigInt>::Cast(args[0]);
  void* handle = reinterpret_cast<void*>(address64->Uint64Value());
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), dlclose(handle)));
}

void spin::GetAddress(const FunctionCallbackInfo<Value> &args) {
  args.GetReturnValue().Set(BigInt::New(args.GetIsolate(), (uint64_t)args[0].As<ArrayBuffer>()->Data()));
}

void spin::Calloc(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  size_t count = Local<Integer>::Cast(args[0])->Value();
  size_t size = 0;
  void* chunk;
  if (args[1]->IsString()) {
    Local<String> str = args[1].As<String>();
    size = str->Utf8Length(isolate);
    chunk = calloc(count, size);
    if (chunk == NULL) return;
    int written;
    char* next = (char*)chunk;
    for (uint32_t i = 0; i < count; i++) {
      str->WriteUtf8(isolate, next, size, &written, 
        String::HINT_MANY_WRITES_EXPECTED | String::NO_NULL_TERMINATION);
      next += written;
    }
  } else {
    if (args[1]->IsBigInt()) {
      size = Local<BigInt>::Cast(args[1])->Uint64Value();
    } else {
      size = Local<Integer>::Cast(args[1])->Value();
    }
    chunk = calloc(count, size);
    if (chunk == NULL) return;
  }
  bool shared = false;
  if (args.Length() > 2) shared = args[2]->BooleanValue(isolate);
  std::unique_ptr<BackingStore> backing;
  if (shared) {
    backing = SharedArrayBuffer::NewBackingStore(chunk, count * size, 
          spin::FreeMemory, nullptr);
    Local<SharedArrayBuffer> ab =
        SharedArrayBuffer::New(isolate, std::move(backing));
    args.GetReturnValue().Set(ab);
  } else {
    backing = ArrayBuffer::NewBackingStore(chunk, count * size, 
        spin::FreeMemory, nullptr);
    Local<ArrayBuffer> ab =
        ArrayBuffer::New(isolate, std::move(backing));
    args.GetReturnValue().Set(ab);
  }
}

void spin::Compile(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  TryCatch try_catch(isolate);
  Local<String> source = args[0].As<String>();
  Local<String> path = args[1].As<String>();
  Local<Array> params_buf = args[2].As<Array>();
  Local<Array> context_extensions_buf;
  context_extensions_buf = args[3].As<Array>();
  std::vector<Local<String>> params;
  if (!params_buf.IsEmpty()) {
    for (uint32_t n = 0; n < params_buf->Length(); n++) {
      Local<Value> val;
      if (!params_buf->Get(context, n).ToLocal(&val)) return;
      params.push_back(val.As<String>());
    }
  }
  std::vector<Local<Object>> context_extensions;
  if (!context_extensions_buf.IsEmpty()) {
    for (uint32_t n = 0; n < context_extensions_buf->Length(); n++) {
      Local<Value> val;
      if (!context_extensions_buf->Get(context, n).ToLocal(&val)) return;
      context_extensions.push_back(val.As<Object>());
    }
  }
  Local<v8::PrimitiveArray> opts =
      v8::PrimitiveArray::New(isolate, 1);
  opts->Set(isolate, 0, v8::Number::New(isolate, 1));
  ScriptOrigin baseorigin(isolate, path, // resource name
    0, // line offset
    0,  // column offset
    false, // is shared cross-origin
    -1,  // script id
    Local<Value>(), // source map url
    false, // is opaque
    false, // is wasm
    false, // is module
    opts);

  Context::Scope scope(context);
  ScriptCompiler::Source basescript(source, baseorigin);
  MaybeLocal<Function> maybe_fn = ScriptCompiler::CompileFunctionInContext(
    context, &basescript, params.size(), params.data(), 0, nullptr, 
    ScriptCompiler::kEagerCompile);
  if (maybe_fn.IsEmpty()) {
    if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
      try_catch.ReThrow();
    }
    return;
  }
  Local<Function> fn = maybe_fn.ToLocalChecked();
  args.GetReturnValue().Set(fn);
}

void spin::ReadFile(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  String::Utf8Value name(isolate, args[1]);
  char* contents = readFile(*name);
  if (contents == NULL) return;
  Local<String> source = String::NewFromUtf8(isolate, contents).ToLocalChecked();
  args.GetReturnValue().Set(source);
}

void spin::ReadString(const FunctionCallbackInfo<Value> &args) {
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  char *data = static_cast<char *>(ab->Data());
  int argc = args.Length();
  int len = 0;
  if (argc > 1) {
    len = Local<Integer>::Cast(args[1])->Value();
  } else {
    len = ab->ByteLength();
  }
  int off = 0;
  if (argc > 2) off = Local<Integer>::Cast(args[2])->Value();
  char* source = data + off;
  args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), source, 
    NewStringType::kNormal, len).ToLocalChecked());
}

void spin::ReadMemory(const FunctionCallbackInfo<Value> &args) {
  Local<BigInt> start64 = Local<BigInt>::Cast(args[0]);
  Local<BigInt> end64 = Local<BigInt>::Cast(args[1]);
  const uint64_t size = end64->Uint64Value() - start64->Uint64Value();
  void* start = reinterpret_cast<void*>(start64->Uint64Value());
  int free = 0;
  if (args.Length() > 2) {
    free = Local<Integer>::Cast(args[2])->Value();
  }
  if (free == 0) {
    std::unique_ptr<BackingStore> backing = ArrayBuffer::NewBackingStore(
        start, size, [](void*, size_t, void*){}, nullptr);
    args.GetReturnValue().Set(ArrayBuffer::New(args.GetIsolate(), std::move(backing)));
    return;
  }
  std::unique_ptr<BackingStore> backing = ArrayBuffer::NewBackingStore(
      start, size, spin::FreeMemory, nullptr);
  args.GetReturnValue().Set(ArrayBuffer::New(args.GetIsolate(), std::move(backing)));
}

void spin::Utf8Length(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  args.GetReturnValue().Set(Integer::New(isolate, 
    args[0].As<String>()->Utf8Length(isolate)));
}

void spin::RawBuffer(const FunctionCallbackInfo<Value> &args) {
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  Local<ArrayBuffer> rb = args[1].As<ArrayBuffer>();
  rawBuffer* buf = static_cast<rawBuffer*>(rb->Data());
  buf->data = (uint8_t*)ab->Data();
  buf->len = ab->ByteLength();
  buf->read = 0;
  buf->written = 0;
  buf->id = bcount;
  buffers[bcount] = buf;
  bcount += 1;
}

void spin::WriteLatin1(const FunctionCallbackInfo<Value> &args) {
  rawBuffer* buf = buffers[Local<Integer>::Cast(args[0])->Value()];
  buf->written = args[1].As<String>()->WriteOneByte(args.GetIsolate(), (uint8_t*)buf->data, 0, buf->len, 
    v8::String::HINT_MANY_WRITES_EXPECTED | 
    v8::String::NO_NULL_TERMINATION
  );
}

void spin::WriteUtf8(const FunctionCallbackInfo<Value> &args) {
  rawBuffer* buf = buffers[Local<Integer>::Cast(args[0])->Value()];
  buf->written = args[1].As<String>()->WriteUtf8(args.GetIsolate(), (char*)buf->data, buf->len, &buf->read, 
    v8::String::HINT_MANY_WRITES_EXPECTED | 
    v8::String::REPLACE_INVALID_UTF8 | 
    v8::String::NO_NULL_TERMINATION
  );
}

void spin::WriteUtf16(const FunctionCallbackInfo<Value> &args) {
  rawBuffer* buf = buffers[Local<Integer>::Cast(args[0])->Value()];
  buf->written = args[1].As<String>()->Write(args.GetIsolate(), (uint16_t*)buf->data, 0, buf->len, 
    v8::String::HINT_MANY_WRITES_EXPECTED | 
    v8::String::NO_NULL_TERMINATION
  );
}

void spin::ReadLatin1(const FunctionCallbackInfo<Value> &args) {
  rawBuffer* buf = buffers[Local<Integer>::Cast(args[0])->Value()];
  args.GetReturnValue().Set(String::NewFromOneByte(args.GetIsolate(), (uint8_t*)buf->data, 
    NewStringType::kNormal, buf->written).ToLocalChecked());
}

void spin::ReadUtf8(const FunctionCallbackInfo<Value> &args) {
  rawBuffer* buf = buffers[Local<Integer>::Cast(args[0])->Value()];
  args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), (char*)buf->data, 
    NewStringType::kNormal, buf->written).ToLocalChecked());
}

void spin::ReadUtf16(const FunctionCallbackInfo<Value> &args) {
  rawBuffer* buf = buffers[Local<Integer>::Cast(args[0])->Value()];
  args.GetReturnValue().Set(String::NewFromTwoByte(args.GetIsolate(), (uint16_t*)buf->data, 
    NewStringType::kNormal, buf->written).ToLocalChecked());
}

void spin::PID(const FunctionCallbackInfo<Value> &args) {
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), getpid()));
}

uint32_t PIDFast(v8::Local<v8::Object> receiver) {
  return getpid();
}

void spin::HRTime(const FunctionCallbackInfo<Value> &args) {
  if (args.Length() > 0) {
    Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
    hrtimeptr = (struct timespec*)ab->Data();
  }
  clock_gettime(clock_id, hrtimeptr);
}

void HRTimeFast(v8::Local<v8::Object> receiver) {
  clock_gettime(clock_id, hrtimeptr);
}

void spin::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> version = ObjectTemplate::New(isolate);
  SET_VALUE(isolate, version, GLOBALOBJ, String::NewFromUtf8Literal(isolate, 
    VERSION));
  SET_VALUE(isolate, version, "v8", String::NewFromUtf8(isolate, 
    v8::V8::GetVersion()).ToLocalChecked());
  Local<ObjectTemplate> kernel = ObjectTemplate::New(isolate);
  utsname kernel_rec;
  int rc = uname(&kernel_rec);
  if (rc == 0) {
    kernel->Set(String::NewFromUtf8Literal(isolate, "os", 
      NewStringType::kNormal), String::NewFromUtf8(isolate, 
      kernel_rec.sysname).ToLocalChecked());
    kernel->Set(String::NewFromUtf8Literal(isolate, "release", 
      NewStringType::kNormal), String::NewFromUtf8(isolate, 
      kernel_rec.release).ToLocalChecked());
    kernel->Set(String::NewFromUtf8Literal(isolate, "version", 
      NewStringType::kNormal), String::NewFromUtf8(isolate, 
      kernel_rec.version).ToLocalChecked());
  }
  version->Set(String::NewFromUtf8Literal(isolate, "kernel", 
    NewStringType::kNormal), kernel);
  SET_MODULE(isolate, target, "version", version);

  SET_METHOD(isolate, target, "nextTick", NextTick);

  SET_METHOD(isolate, target, "compile", Compile);
  SET_METHOD(isolate, target, "memoryUsage", MemoryUsage); // fast

  SET_METHOD(isolate, target, "builtin", Builtin);
  SET_METHOD(isolate, target, "builtins", Builtins);
  SET_METHOD(isolate, target, "modules", Modules);
  SET_METHOD(isolate, target, "load", Load);

  SET_METHOD(isolate, target, "readFile", ReadFile);

  SET_METHOD(isolate, target, "dlopen", DLOpen);
  SET_METHOD(isolate, target, "dlsym", DLSym);
  SET_METHOD(isolate, target, "dlerror", DLError);
  SET_METHOD(isolate, target, "dlclose", DLClose);

  SET_METHOD(isolate, target, "calloc", Calloc);
  SET_METHOD(isolate, target, "readMemory", ReadMemory);
  SET_METHOD(isolate, target, "getAddress", GetAddress);
  SET_METHOD(isolate, target, "readString", ReadString);
  SET_METHOD(isolate, target, "rawBuffer", RawBuffer);

  SET_METHOD(isolate, target, "writeUtf8", WriteUtf8);
  SET_METHOD(isolate, target, "writeLatin1", WriteLatin1);
  SET_METHOD(isolate, target, "writeUtf16", WriteUtf16);
  SET_METHOD(isolate, target, "readUtf8", ReadUtf8);
  SET_METHOD(isolate, target, "readLatin1", ReadLatin1);
  SET_METHOD(isolate, target, "readUtf16", ReadUtf16);

  // readUft8, writeUtf8, readString, writeString, readLatin1String, writeLatin1String
  // C-versions - with/without null terminators
  // ability to pass in an address and just read the bytes 
  SET_METHOD(isolate, target, "utf8Length", Utf8Length);
  SET_METHOD(isolate, target, "print", Print);
  SET_METHOD(isolate, target, "error", Error);

  SET_FAST_METHOD(isolate, target, "pid", PIDFast, PID);
  SET_FAST_METHOD(isolate, target, "hrtime", HRTimeFast, HRTime);

#ifdef __BYTE_ORDER
  // These don't work on alpine. will have to investigate why not
  SET_VALUE(isolate, target, "BYTE_ORDER", v8::Integer::New(isolate, __BYTE_ORDER));
  SET_VALUE(isolate, target, "LITTLE_ENDIAN", v8::Integer::New(isolate, __LITTLE_ENDIAN));
  SET_VALUE(isolate, target, "BIG_ENDIAN", v8::Integer::New(isolate, __BIG_ENDIAN));
#endif
  SET_VALUE(isolate, target, "RTLD_LAZY", v8::Integer::New(isolate, RTLD_LAZY));
  SET_VALUE(isolate, target, "RTLD_NOW", v8::Integer::New(isolate, RTLD_NOW));
}
