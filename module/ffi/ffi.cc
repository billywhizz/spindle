#include <spin.h>
#include <ffi.h>

namespace spin {
namespace ffi {

static ffi_type* spin_ffi_types[16];

void FFIPrepare(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> cb = args[0].As<ArrayBuffer>();
  int rtype = Local<Integer>::Cast(args[1])->Value();
  Local<Array> params = args[2].As<Array>();
  ffi_status status;
  ffi_abi abi = FFI_DEFAULT_ABI;
  // todo: we have to free this
  ffi_cif* cif = (ffi_cif*)calloc(1, sizeof(ffi_cif));
  unsigned int nargs = params->Length();
  // todo: we have to free this
  ffi_type** argtypes = (ffi_type**)calloc(nargs, sizeof(ffi_type));
  for (unsigned int i = 0; i < nargs; i++) {
    Local<Value> p = params->Get(context, i).ToLocalChecked();
    argtypes[i] = spin_ffi_types[Local<Integer>::Cast(p)->Value()];
  }
  status = ffi_prep_cif(cif, abi, nargs, spin_ffi_types[rtype], argtypes);
  if (status != FFI_OK) {
    args.GetReturnValue().Set(Integer::New(isolate, status));
    return;
  }
  cb->SetAlignedPointerInInternalField(1, cif);
  args.GetReturnValue().Set(Integer::New(isolate, status));
}

void FFICall(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  if (!args[0]->IsArrayBuffer() || !args[1]->IsBigInt()) {
    args.GetReturnValue().Set(BigInt::New(isolate, -1));
    return;
  }
  Local<ArrayBuffer> cb = args[0].As<ArrayBuffer>();
  void* fn = reinterpret_cast<void*>(Local<BigInt>::Cast(args[1])->Uint64Value());
  ffi_cif* cif = (ffi_cif*)cb->GetAlignedPointerFromInternalField(1);
  ffi_arg result;
  void** start = (void**)cb->Data();
  ffi_call(cif, FFI_FN(fn), &result, start);
  if (cif->rtype == spin_ffi_types[FFI_TYPE_SINT8]) {
    args.GetReturnValue().Set(Integer::New(isolate, (int8_t)result));
    return;
  }
  if (cif->rtype == spin_ffi_types[FFI_TYPE_SINT16]) {
    args.GetReturnValue().Set(Integer::New(isolate, (int16_t)result));
    return;
  }
  if (cif->rtype == spin_ffi_types[FFI_TYPE_SINT32]) {
    args.GetReturnValue().Set(Integer::New(isolate, (int32_t)result));
    return;
  }
  if (cif->rtype == spin_ffi_types[FFI_TYPE_UINT8]) {
    args.GetReturnValue().Set(Integer::New(isolate, (uint8_t)result));
    return;
  }
  if (cif->rtype == spin_ffi_types[FFI_TYPE_UINT16]) {
    args.GetReturnValue().Set(Integer::New(isolate, (uint16_t)result));
    return;
  }
  if (cif->rtype == spin_ffi_types[FFI_TYPE_UINT32]) {
    args.GetReturnValue().Set(Number::New(isolate, (uint32_t)result));
    return;
  }
  if (cif->rtype == spin_ffi_types[FFI_TYPE_FLOAT]) {
    args.GetReturnValue().Set(Number::New(isolate, (float)result));
    return;
  }
  if (cif->rtype == spin_ffi_types[FFI_TYPE_DOUBLE]) {
    args.GetReturnValue().Set(Number::New(isolate, (double)result));
    return;
  }
  if (cif->rtype == spin_ffi_types[FFI_TYPE_POINTER]) {
    args.GetReturnValue().Set(BigInt::New(isolate, (uint64_t)result));
    return;
  }
  if (cif->rtype == spin_ffi_types[FFI_TYPE_UINT64]) {
    args.GetReturnValue().Set(BigInt::New(isolate, (uint64_t)result));
    return;
  }
  if (cif->rtype == spin_ffi_types[FFI_TYPE_SINT64]) {
    args.GetReturnValue().Set(BigInt::New(isolate, (int64_t)result));
    return;
  }
}

void Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  spin_ffi_types[FFI_TYPE_SINT8] = &ffi_type_sint8;
  spin_ffi_types[FFI_TYPE_SINT16] = &ffi_type_sint16;
  spin_ffi_types[FFI_TYPE_SINT32] = &ffi_type_sint32;
  spin_ffi_types[FFI_TYPE_UINT8] = &ffi_type_uint8;
  spin_ffi_types[FFI_TYPE_UINT16] = &ffi_type_uint16;
  spin_ffi_types[FFI_TYPE_UINT32] = &ffi_type_uint32;
  spin_ffi_types[FFI_TYPE_VOID] = &ffi_type_void;
  spin_ffi_types[FFI_TYPE_FLOAT] = &ffi_type_float;
  spin_ffi_types[FFI_TYPE_DOUBLE] = &ffi_type_double;
  spin_ffi_types[FFI_TYPE_SINT64] = &ffi_type_sint64;
  spin_ffi_types[FFI_TYPE_UINT64] = &ffi_type_uint64;
  spin_ffi_types[FFI_TYPE_POINTER] = &ffi_type_pointer;
  spin_ffi_types[FFI_TYPE_COMPLEX] = &ffi_type_complex_double;
  SET_METHOD(isolate, module, "prepare", FFIPrepare);
  SET_METHOD(isolate, module, "call", FFICall);
  SET_MODULE(isolate, target, "ffi", module);
}

} // namespace ffi
} // namespace spin

extern "C" {
  void* _register_ffi() {
    return (void*)spin::ffi::Init;
  }
}
