
#include <hescape.h>
#include <spin.h>

namespace spin {
namespace html {

Persistent<ObjectTemplate, v8::NonCopyablePersistentTraits<ObjectTemplate>> escaperTpl;

struct escapeBuffer {
  uint8_t* indata;
  uint8_t* outdata;
  uint32_t inlen;
  uint32_t outlen;
};

void escapeLatin1(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  struct escapeBuffer* eb = (struct escapeBuffer*)args.Holder()->GetAlignedPointerFromInternalField(0);
  int32_t written = args[0].As<String>()->WriteOneByte(isolate, 
    eb->indata, 0, eb->inlen,  
    v8::String::HINT_MANY_WRITES_EXPECTED | 
    v8::String::NO_NULL_TERMINATION
  );
  written = hesc_escape_html(eb->outdata, eb->indata, written);
  args.GetReturnValue().Set(String::NewFromOneByte(isolate, 
    eb->outdata, 
    NewStringType::kNormal, written).ToLocalChecked());
}

void escape16(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  struct escapeBuffer* eb = (struct escapeBuffer*)args.Holder()->GetAlignedPointerFromInternalField(0);
  int32_t written = args[0].As<String>()->Write(isolate, 
    (uint16_t*)eb->indata, 0, eb->inlen,  
    v8::String::HINT_MANY_WRITES_EXPECTED | 
    v8::String::NO_NULL_TERMINATION
  );
  written = hesc_escape_html16(eb->outdata, eb->indata, written * 2);
  args.GetReturnValue().Set(String::NewFromTwoByte(isolate, 
    (uint16_t*)eb->outdata, 
    NewStringType::kNormal, written).ToLocalChecked());
}
/*
void escape2(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  struct escapeBuffer* eb = (struct escapeBuffer*)args.Holder()->GetAlignedPointerFromInternalField(0);
  String::Utf8Value str(isolate, args[0]);
  int32_t written = hesc_escape_html(eb->outdata, (const uint8_t*)*str, str.length());
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, 
    (char*)eb->outdata, 
    NewStringType::kNormal, written).ToLocalChecked());
}
*/
void escape(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  struct escapeBuffer* eb = (struct escapeBuffer*)args.Holder()->GetAlignedPointerFromInternalField(0);
  int32_t chars = 0;
  int32_t written = args[0].As<String>()->WriteUtf8(isolate, 
    (char*)eb->indata, eb->inlen, &chars, 
    v8::String::HINT_MANY_WRITES_EXPECTED | v8::String::REPLACE_INVALID_UTF8 | 
    v8::String::NO_NULL_TERMINATION
  );
  written = hesc_escape_html(eb->outdata, eb->indata, written);
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, 
    (char*)eb->outdata, 
    NewStringType::kNormal, written).ToLocalChecked());
}

void createEscaper(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> inb = args[0].As<ArrayBuffer>();
  Local<ArrayBuffer> outb = args[1].As<ArrayBuffer>();
  Local<ObjectTemplate> tpl = Local<ObjectTemplate>::New(isolate, escaperTpl);
  Local<Object> escaper = tpl->NewInstance(context).ToLocalChecked();
  struct escapeBuffer* eb = (struct escapeBuffer*)calloc(1, sizeof(struct escapeBuffer));
  eb->indata = (uint8_t*)inb->Data();
  eb->outdata = (uint8_t*)outb->Data();
  eb->inlen = inb->ByteLength();
  eb->outlen = outb->ByteLength();
  escaper->SetAlignedPointerInInternalField(0, eb);
  args.GetReturnValue().Set(escaper);
}

void escapeHTMLString(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  rawBuffer* inbuf = spin::buffers[Local<Integer>::Cast(args[0])->Value()];
  rawBuffer* outbuf = buffers[Local<Integer>::Cast(args[2])->Value()];
  inbuf->written = args[1].As<String>()->WriteUtf8(isolate, 
    (char*)inbuf->data, inbuf->len, &inbuf->read, 
    v8::String::HINT_MANY_WRITES_EXPECTED | v8::String::REPLACE_INVALID_UTF8 | 
    v8::String::NO_NULL_TERMINATION
  );
  outbuf->written = hesc_escape_html(outbuf->data, inbuf->data, inbuf->written);
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, 
    (char*)outbuf->data, 
    NewStringType::kNormal, outbuf->written).ToLocalChecked());
}

void escapeHTMLSlow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  uint8_t* v0 = reinterpret_cast<uint8_t*>((uint64_t)args[0]->NumberValue(context).ToChecked());
  const uint8_t* v1 = reinterpret_cast<const uint8_t*>((uint64_t)args[1]->NumberValue(context).ToChecked());
  uint32_t v2 = Local<Integer>::Cast(args[2])->Value();
  uint32_t rc = hesc_escape_html(v0, v1, v2);
  args.GetReturnValue().Set(Number::New(isolate, rc));
}

uint32_t escapeHTMLFast(void* p, void* p0, void* p1, uint32_t p2) {
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
  args.GetReturnValue().Set(Number::New(isolate, rc));
}

uint32_t escapeHTML16Fast(void* p, void* p0, void* p1, uint32_t p2) {
  uint8_t* v0 = reinterpret_cast<uint8_t*>(p0);
  const uint8_t* v1 = reinterpret_cast<const uint8_t*>(p1);
  uint32_t v2 = p2;
  return hesc_escape_html16(v0, v1, v2);
}

void Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);

  SET_METHOD(isolate, module, "escapeHtmlString", escapeHTMLString);
  SET_METHOD(isolate, module, "escaper", createEscaper);

  v8::CTypeInfo* cargsescapeHTML = (v8::CTypeInfo*)calloc(4, sizeof(v8::CTypeInfo));
  cargsescapeHTML[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsescapeHTML[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsescapeHTML[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsescapeHTML[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  v8::CTypeInfo* rcescapeHTML = new v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  v8::CFunctionInfo* infoescapeHTML = new v8::CFunctionInfo(*rcescapeHTML, 4, cargsescapeHTML);
  v8::CFunction* pFescapeHTML = new v8::CFunction((const void*)&escapeHTMLFast, infoescapeHTML);
  SET_FAST_METHOD(isolate, module, "escapeHTML", pFescapeHTML, escapeHTMLSlow);

  v8::CTypeInfo* cargsescapeHTML16 = (v8::CTypeInfo*)calloc(4, sizeof(v8::CTypeInfo));
  cargsescapeHTML16[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
  cargsescapeHTML16[1] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsescapeHTML16[2] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint64);
  cargsescapeHTML16[3] = v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  v8::CTypeInfo* rcescapeHTML16 = new v8::CTypeInfo(v8::CTypeInfo::Type::kUint32);
  v8::CFunctionInfo* infoescapeHTML16 = new v8::CFunctionInfo(*rcescapeHTML16, 4, cargsescapeHTML16);
  v8::CFunction* pFescapeHTML16 = new v8::CFunction((const void*)&escapeHTML16Fast, infoescapeHTML16);
  SET_FAST_METHOD(isolate, module, "escapeHTML16", pFescapeHTML16, escapeHTML16Slow);

  Local<ObjectTemplate> tpl = ObjectTemplate::New(isolate);
  tpl->SetInternalFieldCount(1);
  SET_METHOD(isolate, tpl, "escape", escape);
  SET_METHOD(isolate, tpl, "escapeLatin1", escapeLatin1);
  SET_METHOD(isolate, tpl, "escape16", escape16);
  escaperTpl.Reset(isolate, tpl);

  SET_MODULE(isolate, target, "html", module);
}
} // namespace html
} // namespace spin

extern "C" {
  void* _register_html() {
    return (void*)spin::html::Init;
  }
}
