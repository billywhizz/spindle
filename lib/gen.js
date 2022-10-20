function getType (t, rv = false) {
  if (t === 'i8') return 'int8_t'
  if (t === 'i16') return 'int16_t'
  if (t === 'i32') return 'int32_t'
  if (t === 'u8') return 'uint8_t'
  if (t === 'u16') return 'uint16_t'
  if (t === 'u32') return 'uint32_t'
  if (t === 'void') return 'void'
  if (t === 'f32') return 'float'
  if (t === 'f64') return 'double'
  if (t === 'i64') return 'int64_t'
  if (t === 'u64') return 'uint64_t'
  if (t === 'isz') return 'intptr_t'
  if (t === 'usz') return 'uintptr_t'
  if (rv) return 'void'
  return 'void*'
}

function getFastType (id = '') {
  if (id === 'u32') return 'kUint32'
  if (id === 'i32') return 'kInt32'
  if (id === 'pointer') return 'kUint64'
  return 'void'
}

function needsUnwrap (t) {
  if (t === 'u32') return false
  if (t === 'i32') return false
  return true
}

function getParams (def) {
  const pointers = def.pointers.slice(0)
  let params = def.parameters.map((p, i) => `${getType(p)} p${i}`)
  if (needsUnwrap(def.result)) {
    params.push('struct FastApiTypedArray* const p_ret')
  }
  return params.join(', ')
}

function getFastParameterCast (parameter, index, pointers) {
  if (parameter === 'pointer') {
    const pType = pointers[index] || 'void*'
    return `  ${pType} v${index} = reinterpret_cast<${pType}>(p${index});`
  }
  return `  ${getType(parameter)} v${index} = p${index};`
}

function getSlowParameterCast (parameter, index, pointers) {
  if (parameter === 'pointer') {
    const pType = pointers[index] || 'void*'
    return `  ${pType} v${index} = reinterpret_cast<${pType}>((uint64_t)args[${index}]->NumberValue(context).ToChecked());`
  }
  return `  ${getType(parameter)} v${index} = Local<Integer>::Cast(args[${index}])->Value();`
}

function getParameterInit(p, i, name) {
  return `  cargs${name}[${i + 1}] = v8::CTypeInfo(v8::CTypeInfo::Type::${getFastType(p)});`
}

async function bindings (importPath) {
  const { api, includes, name } = await import(importPath)
  const fNames = Object.keys(api)
  for (const name of fNames) {
    const fn = api[name]
    fn.pointers = fn.pointers || []
    fn.name = fn.name || name
  }
  function initFunction (n) {
    const definition = api[n]
    const { parameters, result, name } = definition
    return `
  v8::CTypeInfo* cargs${name} = (v8::CTypeInfo*)calloc(8, sizeof(v8::CTypeInfo));
  cargs${name}[0] = v8::CTypeInfo(v8::CTypeInfo::Type::kV8Value);
${parameters.map((p, i) => getParameterInit(p, i, name)).join('\n')}
  v8::CTypeInfo* rc${name} = new v8::CTypeInfo(v8::CTypeInfo::Type::${getFastType(result)});
  v8::CFunctionInfo* info${name} = new v8::CFunctionInfo(*rc${name}, ${parameters.length + 1}, cargs${name});
  v8::CFunction* pF${name} = new v8::CFunction((const void*)&${name}Fast, info${name});
  SET_FAST_METHOD(isolate, module, "${name}", pF${name}, ${name}Slow);`
  }

  function getFunction (n) {
    const definition = api[n]
    const { parameters, pointers, result, name } = definition
    let src = `
void ${name}Slow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();`
    if (parameters.includes('pointer')) {
      src += `\n  Local<Context> context = isolate->GetCurrentContext();\n`
    }
    src += `${parameters.map((p, i) => getSlowParameterCast(p, i, pointers)).join('\n')}
  ${getType(result)} rc = ${n}(${parameters.map((p, i) => `v${i}`).join(', ')});
`
    if (needsUnwrap(result)) {
      if (result === 'pointer') {
        src += `  args.GetReturnValue().Set(Number::New(isolate, reinterpret_cast<uint64_t>(rc)));\n`
      }
    } else {
      src += `  args.GetReturnValue().Set(Number::New(isolate, rc));\n`
    }
    src += `}

${getType(result, true)} ${name}Fast(void* p${parameters.length ? ', ' : ''}${getParams(definition)}) {
${parameters.map((p, i) => getFastParameterCast(p, i, pointers)).join('\n')}`
    if (needsUnwrap(result)) {
      src += `\n  ${getType(result)} r = ${n}(${parameters.map((p, i) => `v${i}`).join(', ')});\n`
      src += `  ((${getType(result)}*)p_ret->data)[0] = r;\n`
    } else {
      src += `\n  return ${n}(${parameters.map((p, i) => `v${i}`).join(', ')});`
    }
    src += '\n}'
    return src
  }

  return `${includes.map(include => `#include <${include}>`).join('\n')}
#include <spin.h>

namespace spin {
namespace ${name} {
${fNames.map(getFunction).join('\n')}

void Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
${fNames.map(initFunction).join('\n')}
  SET_MODULE(isolate, target, "${name}", module);
}
} // namespace ${name}
} // namespace spin

extern "C" {
  void* _register_${name}() {
    return (void*)spin::${name}::Init;
  }
}`
}

const rx = /[./-]/g

function linkerScript (fileName) {
  const name = `_binary_${fileName.replace(rx, '_')}`
  return `.global ${name}_start
${name}_start:
        .incbin "${fileName}"
        .global ${name}_end
${name}_end:
`
}

function baseName (path) {
  return path.slice(path.lastIndexOf('/') + 1, path.lastIndexOf('.'))
}

function fileName (path) {
  return path.slice(path.lastIndexOf('/') + 1)
}

function extName (path) {
  const pos = path.lastIndexOf('.')
  if (pos < 0) return ''
  return path.slice(pos + 1)
}

function headerFile (deps = [], v8flags = '--stack-trace-limit=10 --use-strict --turbo-fast-api-calls --single-threaded --nolazy') {
  const libs = deps.filter(dep => extName(dep) !== 'a')
  const modules = deps.filter(dep => extName(dep) === 'a')
  let source = `#pragma once

#include "spin.h"

using v8::V8;
using v8::Platform;

extern char _binary_main_js_start[];
extern char _binary_main_js_end[];
`
  for (const lib of libs) {
    const name = `_binary_${lib.replace(rx, '_')}`
    source += `extern char ${name}_start[];\n`
    source += `extern char ${name}_end[];\n`
  }
  if (modules.length) {
    source += '\nextern "C" {\n'
  }
  for (const module of modules) {
    source += `  extern void* _register_${baseName(module)}();\n`;
  }
  if (modules.length) source += '}\n'
  source += `
void register_builtins() {
  spin::builtins_add("main.js", _binary_main_js_start, _binary_main_js_end - _binary_main_js_start);
`
  for (const lib of libs) {
    const name = `_binary_${lib.replace(rx, '_')}`
    source += `  spin::builtins_add("${lib}", ${name}_start, ${name}_end - ${name}_start);\n`
  }
  for (const module of modules) {
    const name = baseName(module)
    source += `  spin::modules["${name}"] = &_register_${name};\n`;
  }
  source += `}

static unsigned int main_js_len = _binary_main_js_end - _binary_main_js_start;
static const char* main_js = _binary_main_js_start;
static const char* v8flags = "${v8flags}";
static unsigned int _v8flags_from_commandline = ${v8flags ? 1 : 0};
`
  return source
}

export { bindings, linkerScript, headerFile }