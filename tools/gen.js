function getType (id = '') {
  if (id === 'u32') return 'uint32_t'
  if (id === 'i32') return 'int32_t'
  if (id === 'pointer') return 'uint64_t'
  return 'void'
}

function getFastType (id = '') {
  if (id === 'u32') return 'kUint32'
  if (id === 'i32') return 'kInt32'
  if (id === 'pointer') return 'kUint64'
  return 'void'
}

function getParams (def) {
  const pointers = def.pointers.slice(0)
  return def.parameters.map((p, i) => `${getType(p, pointers)} p${i}`).join(', ')
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

async function generate (importPath) {
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
    const src = `
void ${name}Slow(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
${parameters.map((p, i) => getSlowParameterCast(p, i, pointers)).join('\n')}
  ${getType(result)} rc = ${n}(${parameters.map((p, i) => `v${i}`).join(', ')});
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}  

${getType(result)} ${name}Fast(void* p${parameters.length ? ', ' : ''}${getParams(definition)}) {
${parameters.map((p, i) => getFastParameterCast(p, i, pointers)).join('\n')}
  return ${n}(${parameters.map((p, i) => `v${i}`).join(', ')});
}`
    return src
  }

  return `
${includes.map(include => `#include <${include}>`).join('\n')}
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
}

`
}


export { generate }
