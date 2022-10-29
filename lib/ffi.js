function genFunc (name, params, rtype, cif, address, dv, size) {
  const stack = new ArrayBuffer(size)
  const rdv = new DataView(stack)
  const raddr = spin.getAddress(stack)
  let pos = BigInt(raddr)
  let off = 0
  for (let i = 0; i < params.length; i++) {
    dv.setBigUint64(off, pos, true)
    off += 8
    pos += BigInt(paramSize(params[i]))
  }
  const source = []
  for (let i = 0; i < params.length; i++) {
    source.push(`let l${i}`)
  }
  source.push(`function f (${params.map((v, i) => `p${i}`).join(', ')}${isReturnedAsBigInt(rtype) ? (params.length ? ', vi': 'vi'): ''}) {`)
  let poff = 0
  for (let i = 0; i < params.length; i++) {
    const psize = paramSize(params[i])
    switch (params[i]) {
      case Types.function:
      case Types.buffer:
      case Types.u32array:
        source.push(`  if (p${i} !== l${i}) {`)
        source.push(`    rdv.setBigUint64(${poff}, BigInt(getAddress(p${i}.buffer)), true);`)
        source.push(`    l${i} = p${i}`)
        source.push('  }')
        break;
      case Types.i64:
      case Types.iSize:
        source.push(`  if (p${i} !== l${i}) {`)
        source.push(`    rdv.setBigInt64(${poff}, p${i}, true);`)
        source.push(`    l${i} = p${i}`)
        source.push('  }')
        break;
      case Types.u64:
      case Types.uSize:
      case Types.pointer:
        source.push(`  if (p${i} !== l${i}) {`)
        source.push(`    rdv.setBigUint64(${poff}, BigInt(p${i}), true);`)
        source.push(`    l${i} = p${i}`)
        source.push('  }')
        break;
      case Types.f64:
        source.push(`  if (p${i} !== l${i}) {`)
        source.push(`    rdv.setFloat64(${poff}, p${i}, true);`)
        source.push(`    l${i} = p${i}`)
        source.push('  }')
        break;
      case Types.f32:
        source.push(`  if (p${i} !== l${i}) {`)
        source.push(`    rdv.setFloat32(${poff}, p${i}, true);`)
        source.push(`    l${i} = p${i}`)
        source.push('  }')
        break;
      case Types.i32:
        source.push(`  if (p${i} !== l${i}) {`)
        source.push(`    rdv.setInt32(${poff}, p${i}, true);`)
        source.push(`    l${i} = p${i}`)
        source.push('  }')
        break;
      case Types.u32:
        source.push(`  if (p${i} !== l${i}) {`)
        source.push(`    rdv.setUint32(${poff}, p${i}, true);`)
        source.push(`    l${i} = p${i}`)
        source.push('  }')
        break;
    }
    poff += psize
  }
  if (isReturnedAsBigInt(rtype)) {
    source.push('  const r = ffi.call(cif, BigInt(address))')
    source.push('  vi[0] = Number(r & 0xffffffffn)')
    source.push('  vi[1] = Number(r >> 32n)')
    source.push('  return')
  } else {
    source.push('  return ffi.call(cif, BigInt(address))')
  }
  source.push('}')
  source.push('return f')
  //console.log(`${name}\n${source.join('\n').trim()}`)
  return spin.compile(source.join('\n').trim(), `func.js`, ['cif', 'address', 'rdv', 'getAddress', 'ffi'], [])(cif, address, rdv, spin.getAddress, spin.ffi)
}

const FFI_TYPE_VOID       = 0
const FFI_TYPE_INT        = 1
const FFI_TYPE_FLOAT      = 2
const FFI_TYPE_DOUBLE     = 3
const FFI_TYPE_LONGDOUBLE = 4
const FFI_TYPE_UINT8      = 5
const FFI_TYPE_SINT8      = 6
const FFI_TYPE_UINT16     = 7
const FFI_TYPE_SINT16     = 8
const FFI_TYPE_UINT32     = 9
const FFI_TYPE_SINT32     = 10
const FFI_TYPE_UINT64     = 11
const FFI_TYPE_SINT64     = 12
const FFI_TYPE_STRUCT     = 13
const FFI_TYPE_POINTER    = 14
const FFI_TYPE_COMPLEX    = 15

const FFI_OK = 0
const Types = {
  i8: 1,
  i16: 2,
  i32: 'i32',
  u8: 4,
  u16: 5,
  u32: 6,
  void: 7,
  f32: 8,
  f64: 9,
  u64: 10,
  i64: 11,
  iSize: 12,
  uSize: 13,
  pointer: 'pointer',
  buffer: 15,
  function: 16,
  u32array: 17,
  bool: 18
}

function translateType (t) {
  return Types[t]
}

function FFIArgtypeFromType (t) {
  if (t === Types.i8) return FFI_TYPE_SINT8
  if (t === Types.i16) return FFI_TYPE_SINT16
  if (t === Types.i32) return FFI_TYPE_SINT32
  if (t === Types.u8) return FFI_TYPE_UINT8
  if (t === Types.u16) return FFI_TYPE_UINT16
  if (t === Types.u32) return FFI_TYPE_UINT32
  if (t === Types.void) return FFI_TYPE_VOID
  if (t === Types.f32) return FFI_TYPE_FLOAT
  if (t === Types.f64) return FFI_TYPE_DOUBLE
  if (t === Types.i64) return FFI_TYPE_SINT64
  if (t === Types.u64) return FFI_TYPE_UINT64
  if (t === Types.iSize) return FFI_TYPE_SINT64
  if (t === Types.uSize) return FFI_TYPE_UINT64
  if (t === Types.buffer) return FFI_TYPE_POINTER
  if (t === Types.u32array) return FFI_TYPE_POINTER
  return FFI_TYPE_POINTER
}

function paramSize (t) {
  if (t === Types.i8) return 1
  if (t === Types.i16) return 2
  if (t === Types.i32) return 4
  if (t === Types.u8) return 1
  if (t === Types.u16) return 2
  if (t === Types.u32) return 4
  if (t === Types.void) return 8
  if (t === Types.f32) return 4
  if (t === Types.f64) return 8
  if (t === Types.i64) return 8
  if (t === Types.u64) return 8
  if (t === Types.iSize) return 8
  if (t === Types.uSize) return 8
  if (t === Types.buffer) return 8
  if (t === Types.u32array) return 8
  return 8
}

function paramsSize (params) {
  let size = 0
  for (let i = 0; i < params.length; i++) {
    size += paramSize(params[i])
  }
  return size
}


function isPointerType(type) {
  return type === "buffer" || type === "pointer" ||
    typeof type === "object" && type !== null && "function" in type
}

function isReturnedAsBigInt(type) {
  return isPointerType(type) || type === "u64" || type === "i64" ||
    type === "uSize" || type === "iSize"
}

function isI64(type) {
  return type === "i64" || type === 'u64' || type === "pointer"
}

function wrapBigIntReturnFn (fn, parameters, result) {
  const call = fn
  const vi = new Int32Array(2);
  const vui = new Uint32Array(vi.buffer);
  const b = new BigInt64Array(vi.buffer);
  const params = parameters.map((_, index) => `p${index}`).join(', ')
  const f = new Function(
    "vi",
    "vui",
    "b",
    "call",
    `return function (${params}) {
    call(${params}${parameters.length > 0 ? ", " : ""}vi);
    ${
      isI64(result)
        ? `const n1 = Number(b[0])`
        : `const n1 = vui[0] + 2 ** 32 * vui[1]` // Faster path for u64
    };
    if (Number.isSafeInteger(n1)) return n1;
    return b[0];
  }`,)
  return f(vi, vui, b, call)
}

function wrap (name, address, rtype, params) {
  const dv = new DataView(new ArrayBuffer(8 * params.length))
  const cif = dv.buffer
  const status = spin.ffi.prepare(cif, FFIArgtypeFromType(translateType(rtype)), params.map(FFIArgtypeFromType))
  if (status !== FFI_OK) throw new Error(`Bad Status ${status}`)
  const size = paramsSize(params)
  if (size === 0 && !isReturnedAsBigInt(rtype)) return () => spin.ffi.call(cif, address)
  if (!isReturnedAsBigInt(rtype)) return genFunc(name, params, rtype, cif, address, dv, size)
  return wrapBigIntReturnFn(genFunc(name, params, rtype, cif, address, dv, size), params, rtype)
}

const ffi = { wrap }

export { ffi }
