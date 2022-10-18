
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <spin.h>

namespace spin {
namespace net {

void socketSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  int32_t v1 = Local<Integer>::Cast(args[1])->Value();
  int32_t v2 = Local<Integer>::Cast(args[2])->Value();
  int32_t rc = socket(v0, v1, v2);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t socketFast(void* p, int32_t p0, int32_t p1, int32_t p2) {
  int32_t v0 = p0;
  int32_t v1 = p1;
  int32_t v2 = p2;
  return socket(v0, v1, v2);
}

void setsockoptSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  int32_t v1 = Local<Integer>::Cast(args[1])->Value();
  int32_t v2 = Local<Integer>::Cast(args[2])->Value();
  void* v3 = reinterpret_cast<void*>((uint64_t)args[3]->NumberValue(context).ToChecked());
  int32_t v4 = Local<Integer>::Cast(args[4])->Value();
  int32_t rc = setsockopt(v0, v1, v2, v3, v4);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t setsockoptFast(void* p, int32_t p0, int32_t p1, int32_t p2, uint64_t p3, int32_t p4) {
  int32_t v0 = p0;
  int32_t v1 = p1;
  int32_t v2 = p2;
  void* v3 = reinterpret_cast<void*>(p3);
  int32_t v4 = p4;
  return setsockopt(v0, v1, v2, v3, v4);
}

void bindSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  const sockaddr* v1 = reinterpret_cast<const sockaddr*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  int32_t v2 = Local<Integer>::Cast(args[2])->Value();
  int32_t rc = bind(v0, v1, v2);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t bindFast(void* p, int32_t p0, uint64_t p1, int32_t p2) {
  int32_t v0 = p0;
  const sockaddr* v1 = reinterpret_cast<const sockaddr*>(p1);
  int32_t v2 = p2;
  return bind(v0, v1, v2);
}

void connectSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  const sockaddr* v1 = reinterpret_cast<const sockaddr*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  int32_t v2 = Local<Integer>::Cast(args[2])->Value();
  int32_t rc = connect(v0, v1, v2);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t connectFast(void* p, int32_t p0, uint64_t p1, int32_t p2) {
  int32_t v0 = p0;
  const sockaddr* v1 = reinterpret_cast<const sockaddr*>(p1);
  int32_t v2 = p2;
  return connect(v0, v1, v2);
}

void listenSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  int32_t v1 = Local<Integer>::Cast(args[1])->Value();
  int32_t rc = listen(v0, v1);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t listenFast(void* p, int32_t p0, int32_t p1) {
  int32_t v0 = p0;
  int32_t v1 = p1;
  return listen(v0, v1);
}

void closeSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  int32_t rc = close(v0);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t closeFast(void* p, int32_t p0) {
  int32_t v0 = p0;
  return close(v0);
}

void accept4Slow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  sockaddr* v1 = reinterpret_cast<sockaddr*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  socklen_t* v2 = reinterpret_cast<socklen_t*>((uint64_t)args[2]->NumberValue(context).ToChecked());
  int32_t v3 = Local<Integer>::Cast(args[3])->Value();
  int32_t rc = accept4(v0, v1, v2, v3);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t accept4Fast(void* p, int32_t p0, uint64_t p1, uint64_t p2, int32_t p3) {
  int32_t v0 = p0;
  sockaddr* v1 = reinterpret_cast<sockaddr*>(p1);
  socklen_t* v2 = reinterpret_cast<socklen_t*>(p2);
  int32_t v3 = p3;
  return accept4(v0, v1, v2, v3);
}

void sendSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  void* v1 = reinterpret_cast<void*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  int32_t v2 = Local<Integer>::Cast(args[2])->Value();
  int32_t v3 = Local<Integer>::Cast(args[3])->Value();
  int32_t rc = send(v0, v1, v2, v3);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t sendFast(void* p, int32_t p0, uint64_t p1, int32_t p2, int32_t p3) {
  int32_t v0 = p0;
  void* v1 = reinterpret_cast<void*>(p1);
  int32_t v2 = p2;
  int32_t v3 = p3;
  return send(v0, v1, v2, v3);
}

void recvSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  void* v1 = reinterpret_cast<void*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  int32_t v2 = Local<Integer>::Cast(args[2])->Value();
  int32_t v3 = Local<Integer>::Cast(args[3])->Value();
  int32_t rc = recv(v0, v1, v2, v3);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t recvFast(void* p, int32_t p0, uint64_t p1, int32_t p2, int32_t p3) {
  int32_t v0 = p0;
  void* v1 = reinterpret_cast<void*>(p1);
  int32_t v2 = p2;
  int32_t v3 = p3;
  return recv(v0, v1, v2, v3);
}

void readSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  void* v1 = reinterpret_cast<void*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  int32_t v2 = Local<Integer>::Cast(args[2])->Value();
  int32_t rc = read(v0, v1, v2);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t readFast(void* p, int32_t p0, uint64_t p1, int32_t p2) {
  int32_t v0 = p0;
  void* v1 = reinterpret_cast<void*>(p1);
  int32_t v2 = p2;
  return read(v0, v1, v2);
}

void pipe2Slow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int* v0 = reinterpret_cast<int*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  int32_t v1 = Local<Integer>::Cast(args[1])->Value();
  int32_t rc = pipe2(v0, v1);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t pipe2Fast(void* p, uint64_t p0, int32_t p1) {
  int* v0 = reinterpret_cast<int*>(p0);
  int32_t v1 = p1;
  return pipe2(v0, v1);
}

void dup2Slow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int32_t v0 = Local<Integer>::Cast(args[0])->Value();
  int32_t v1 = Local<Integer>::Cast(args[1])->Value();
  int32_t rc = dup2(v0, v1);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t dup2Fast(void* p, int32_t p0, int32_t p1) {
  int32_t v0 = p0;
  int32_t v1 = p1;
  return dup2(v0, v1);
}

void Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);

  v8::CTypeInfo* cargssocket = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargssocket[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargssocket[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargssocket[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargssocket[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcsocket = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infosocket = new v8::CFunctionInfo(*rcsocket, 4, cargssocket);
  v8::CFunction* pFsocket = new v8::CFunction((const void*)&socketFast, infosocket);
  SET_FAST_METHOD(isolate, module, "socket", pFsocket, socketSlow);

  v8::CTypeInfo* cargssetsockopt = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargssetsockopt[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargssetsockopt[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargssetsockopt[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargssetsockopt[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargssetsockopt[4] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargssetsockopt[5] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcsetsockopt = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infosetsockopt = new v8::CFunctionInfo(*rcsetsockopt, 6, cargssetsockopt);
  v8::CFunction* pFsetsockopt = new v8::CFunction((const void*)&setsockoptFast, infosetsockopt);
  SET_FAST_METHOD(isolate, module, "setsockopt", pFsetsockopt, setsockoptSlow);

  v8::CTypeInfo* cargsbind = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsbind[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsbind[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargsbind[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsbind[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcbind = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infobind = new v8::CFunctionInfo(*rcbind, 4, cargsbind);
  v8::CFunction* pFbind = new v8::CFunction((const void*)&bindFast, infobind);
  SET_FAST_METHOD(isolate, module, "bind", pFbind, bindSlow);

  v8::CTypeInfo* cargsconnect = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsconnect[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsconnect[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargsconnect[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsconnect[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcconnect = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infoconnect = new v8::CFunctionInfo(*rcconnect, 4, cargsconnect);
  v8::CFunction* pFconnect = new v8::CFunction((const void*)&connectFast, infoconnect);
  SET_FAST_METHOD(isolate, module, "connect", pFconnect, connectSlow);

  v8::CTypeInfo* cargslisten = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargslisten[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargslisten[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargslisten[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rclisten = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infolisten = new v8::CFunctionInfo(*rclisten, 3, cargslisten);
  v8::CFunction* pFlisten = new v8::CFunction((const void*)&listenFast, infolisten);
  SET_FAST_METHOD(isolate, module, "listen", pFlisten, listenSlow);

  v8::CTypeInfo* cargsclose = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsclose[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsclose[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcclose = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infoclose = new v8::CFunctionInfo(*rcclose, 2, cargsclose);
  v8::CFunction* pFclose = new v8::CFunction((const void*)&closeFast, infoclose);
  SET_FAST_METHOD(isolate, module, "close", pFclose, closeSlow);

  v8::CTypeInfo* cargsaccept4 = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsaccept4[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsaccept4[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargsaccept4[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsaccept4[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsaccept4[4] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcaccept4 = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infoaccept4 = new v8::CFunctionInfo(*rcaccept4, 5, cargsaccept4);
  v8::CFunction* pFaccept4 = new v8::CFunction((const void*)&accept4Fast, infoaccept4);
  SET_FAST_METHOD(isolate, module, "accept4", pFaccept4, accept4Slow);

  v8::CTypeInfo* cargssend = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargssend[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargssend[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargssend[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargssend[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargssend[4] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcsend = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infosend = new v8::CFunctionInfo(*rcsend, 5, cargssend);
  v8::CFunction* pFsend = new v8::CFunction((const void*)&sendFast, infosend);
  SET_FAST_METHOD(isolate, module, "send", pFsend, sendSlow);

  v8::CTypeInfo* cargsrecv = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsrecv[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsrecv[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargsrecv[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsrecv[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargsrecv[4] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcrecv = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* inforecv = new v8::CFunctionInfo(*rcrecv, 5, cargsrecv);
  v8::CFunction* pFrecv = new v8::CFunction((const void*)&recvFast, inforecv);
  SET_FAST_METHOD(isolate, module, "recv", pFrecv, recvSlow);

  v8::CTypeInfo* cargsread = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsread[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsread[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargsread[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsread[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcread = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* inforead = new v8::CFunctionInfo(*rcread, 4, cargsread);
  v8::CFunction* pFread = new v8::CFunction((const void*)&readFast, inforead);
  SET_FAST_METHOD(isolate, module, "read", pFread, readSlow);

  v8::CTypeInfo* cargspipe2 = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargspipe2[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargspipe2[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargspipe2[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcpipe2 = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infopipe2 = new v8::CFunctionInfo(*rcpipe2, 3, cargspipe2);
  v8::CFunction* pFpipe2 = new v8::CFunction((const void*)&pipe2Fast, infopipe2);
  SET_FAST_METHOD(isolate, module, "pipe2", pFpipe2, pipe2Slow);

  v8::CTypeInfo* cargsdup2 = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsdup2[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsdup2[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  cargsdup2[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CTypeInfo* rcdup2 = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infodup2 = new v8::CFunctionInfo(*rcdup2, 3, cargsdup2);
  v8::CFunction* pFdup2 = new v8::CFunction((const void*)&dup2Fast, infodup2);
  SET_FAST_METHOD(isolate, module, "dup2", pFdup2, dup2Slow);
  SET_MODULE(isolate, target, "net", module);
}
} // namespace net
} // namespace spin

extern "C" {
  void* _register_net() {
    return (void*)spin::net::Init;
  }
}


