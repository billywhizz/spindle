
#include <pico.h>
#include <hescape.h>
#include <spin.h>

namespace spin {
namespace pico {

void zlibDeflateSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  uint8_t* v0 = reinterpret_cast<uint8_t*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  uint32_t v1 = Local<Integer>::Cast(args[1])->Value();
  uint8_t* v2 = reinterpret_cast<uint8_t*>((uint64_t)args[2]->NumberValue(context).ToChecked());
  uint32_t v3 = Local<Integer>::Cast(args[3])->Value();
  uint32_t rc = zlib_deflate(v0, v1, v2, v3);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

uint32_t zlibDeflateFast(void* p, uint64_t p0, uint32_t p1, uint64_t p2, uint32_t p3) {
  uint8_t* v0 = reinterpret_cast<uint8_t*>(p0);
  uint32_t v1 = p1;
  uint8_t* v2 = reinterpret_cast<uint8_t*>(p2);
  uint32_t v3 = p3;
  return zlib_deflate(v0, v1, v2, v3);
}

void zlibInflateSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  uint8_t* v0 = reinterpret_cast<uint8_t*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  uint32_t v1 = Local<Integer>::Cast(args[1])->Value();
  uint8_t* v2 = reinterpret_cast<uint8_t*>((uint64_t)args[2]->NumberValue(context).ToChecked());
  uint32_t v3 = Local<Integer>::Cast(args[3])->Value();
  uint32_t rc = zlib_inflate(v0, v1, v2, v3);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

uint32_t zlibInflateFast(void* p, uint64_t p0, uint32_t p1, uint64_t p2, uint32_t p3) {
  uint8_t* v0 = reinterpret_cast<uint8_t*>(p0);
  uint32_t v1 = p1;
  uint8_t* v2 = reinterpret_cast<uint8_t*>(p2);
  uint32_t v3 = p3;
  return zlib_inflate(v0, v1, v2, v3);
}

void parseRequestSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  char* v0 = reinterpret_cast<char*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  uint32_t v1 = Local<Integer>::Cast(args[1])->Value();
  httpRequest* v2 = reinterpret_cast<httpRequest*>((uint64_t)args[2]->NumberValue(context).ToChecked());
  int32_t rc = parse_request(v0, v1, v2);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t parseRequestFast(void* p, uint64_t p0, uint32_t p1, uint64_t p2) {
  char* v0 = reinterpret_cast<char*>(p0);
  uint32_t v1 = p1;
  httpRequest* v2 = reinterpret_cast<httpRequest*>(p2);
  return parse_request(v0, v1, v2);
}

void parseResponseSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  char* v0 = reinterpret_cast<char*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  uint32_t v1 = Local<Integer>::Cast(args[1])->Value();
  httpResponse* v2 = reinterpret_cast<httpResponse*>((uint64_t)args[2]->NumberValue(context).ToChecked());
  int32_t rc = parse_response(v0, v1, v2);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

int32_t parseResponseFast(void* p, uint64_t p0, uint32_t p1, uint64_t p2) {
  char* v0 = reinterpret_cast<char*>(p0);
  uint32_t v1 = p1;
  httpResponse* v2 = reinterpret_cast<httpResponse*>(p2);
  return parse_response(v0, v1, v2);
}

void escapeHTMLSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  uint8_t* v0 = reinterpret_cast<uint8_t*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  const uint8_t* v1 = reinterpret_cast<const uint8_t*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  uint32_t v2 = Local<Integer>::Cast(args[2])->Value();
  uint32_t rc = hesc_escape_html(v0, v1, v2);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

uint32_t escapeHTMLFast(void* p, uint64_t p0, uint64_t p1, uint32_t p2) {
  uint8_t* v0 = reinterpret_cast<uint8_t*>(p0);
  const uint8_t* v1 = reinterpret_cast<const uint8_t*>(p1);
  uint32_t v2 = p2;
  return hesc_escape_html(v0, v1, v2);
}

void escapeHTML16Slow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  uint8_t* v0 = reinterpret_cast<uint8_t*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  const uint8_t* v1 = reinterpret_cast<const uint8_t*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  uint32_t v2 = Local<Integer>::Cast(args[2])->Value();
  uint32_t rc = hesc_escape_html16(v0, v1, v2);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

uint32_t escapeHTML16Fast(void* p, uint64_t p0, uint64_t p1, uint32_t p2) {
  uint8_t* v0 = reinterpret_cast<uint8_t*>(p0);
  const uint8_t* v1 = reinterpret_cast<const uint8_t*>(p1);
  uint32_t v2 = p2;
  return hesc_escape_html16(v0, v1, v2);
}

void Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);

  v8::CTypeInfo* cargszlibDeflate = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargszlibDeflate[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargszlibDeflate[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargszlibDeflate[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  cargszlibDeflate[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargszlibDeflate[4] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  v8::CTypeInfo* rczlibDeflate = new v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  v8::CFunctionInfo* infozlibDeflate = new v8::CFunctionInfo(*rczlibDeflate, 5, cargszlibDeflate);
  v8::CFunction* pFzlibDeflate = new v8::CFunction((const void*)&zlibDeflateFast, infozlibDeflate);
  SET_FAST_METHOD(isolate, module, "zlibDeflate", pFzlibDeflate, zlibDeflateSlow);

  v8::CTypeInfo* cargszlibInflate = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargszlibInflate[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargszlibInflate[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargszlibInflate[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  cargszlibInflate[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargszlibInflate[4] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  v8::CTypeInfo* rczlibInflate = new v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  v8::CFunctionInfo* infozlibInflate = new v8::CFunctionInfo(*rczlibInflate, 5, cargszlibInflate);
  v8::CFunction* pFzlibInflate = new v8::CFunction((const void*)&zlibInflateFast, infozlibInflate);
  SET_FAST_METHOD(isolate, module, "zlibInflate", pFzlibInflate, zlibInflateSlow);

  v8::CTypeInfo* cargsparseRequest = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsparseRequest[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsparseRequest[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsparseRequest[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  cargsparseRequest[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  v8::CTypeInfo* rcparseRequest = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infoparseRequest = new v8::CFunctionInfo(*rcparseRequest, 4, cargsparseRequest);
  v8::CFunction* pFparseRequest = new v8::CFunction((const void*)&parseRequestFast, infoparseRequest);
  SET_FAST_METHOD(isolate, module, "parseRequest", pFparseRequest, parseRequestSlow);

  v8::CTypeInfo* cargsparseResponse = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsparseResponse[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsparseResponse[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsparseResponse[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  cargsparseResponse[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  v8::CTypeInfo* rcparseResponse = new v8::CTypeInfo(v8::CTypeInfo::Type::kInt32);
  v8::CFunctionInfo* infoparseResponse = new v8::CFunctionInfo(*rcparseResponse, 4, cargsparseResponse);
  v8::CFunction* pFparseResponse = new v8::CFunction((const void*)&parseResponseFast, infoparseResponse);
  SET_FAST_METHOD(isolate, module, "parseResponse", pFparseResponse, parseResponseSlow);

  v8::CTypeInfo* cargsescapeHTML = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsescapeHTML[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsescapeHTML[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsescapeHTML[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsescapeHTML[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  v8::CTypeInfo* rcescapeHTML = new v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  v8::CFunctionInfo* infoescapeHTML = new v8::CFunctionInfo(*rcescapeHTML, 4, cargsescapeHTML);
  v8::CFunction* pFescapeHTML = new v8::CFunction((const void*)&escapeHTMLFast, infoescapeHTML);
  SET_FAST_METHOD(isolate, module, "escapeHTML", pFescapeHTML, escapeHTMLSlow);

  v8::CTypeInfo* cargsescapeHTML16 = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargsescapeHTML16[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsescapeHTML16[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsescapeHTML16[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsescapeHTML16[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  v8::CTypeInfo* rcescapeHTML16 = new v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  v8::CFunctionInfo* infoescapeHTML16 = new v8::CFunctionInfo(*rcescapeHTML16, 4, cargsescapeHTML16);
  v8::CFunction* pFescapeHTML16 = new v8::CFunction((const void*)&escapeHTML16Fast, infoescapeHTML16);
  SET_FAST_METHOD(isolate, module, "escapeHTML16", pFescapeHTML16, escapeHTML16Slow);
  SET_MODULE(isolate, target, "pico", module);
}
} // namespace pico
} // namespace spin

extern "C" {
  void* _register_pico() {
    return (void*)spin::pico::Init;
  }
}


