const AD = '\u001b[0m' // ANSI Default
const A0 = '\u001b[30m' // ANSI Black
const AR = '\u001b[31m' // ANSI Red
const AG = '\u001b[32m' // ANSI Green
const AY = '\u001b[33m' // ANSI Yellow
const AB = '\u001b[34m' // ANSI Blue
const AM = '\u001b[35m' // ANSI Magenta
const AC = '\u001b[36m' // ANSI Cyan
const AW = '\u001b[37m' // ANSI White

spin.colors = { AD, AB, A0, AR, AG, AY, AM, AC, AW }

function assert (condition, message) {
  if (!condition) {
    throw new Error(message || "Assertion failed")
  }
}

const replacer = (k, v) => {
  return (typeof v === 'bigint') 
    ? (v < Number.MAX_SAFE_INTEGER ? Number(v) : `b${v.toString()}`) 
    : v
}

spin.stringify = (o, sp = '  ') => JSON.stringify(o, replacer, sp)

function CString (str) {
  const buf = spin.calloc(1, `${str}\0`)
  buf.ptr = spin.getAddress(buf)
  return buf
}

spin.CString = CString
spin.assert = assert

global.console = {
  log: str => spin.print(str),
  error: str => spin.error(str)
}

global.onUnhandledRejection = err => {
  console.error(`${AR}${err.message}${AD}\n${err.stack}`)
  // todo: exit? maybe with a flag
}

// todo: raw buffer from pointer with size - no arraybuffer, just an address
class RawBuffer {
  constructor (size) {
    this.buf = new ArrayBuffer(size)
    this.ptr = spin.getAddress(this.buf)
    this.state = new Uint32Array(6)
    this.u8 = new Uint8Array(this.buf)
    spin.rawBuffer(this.buf, this.state.buffer)
    this.state[2] = this.state[0]
  }

  get size () {
    return this.state[0]
  }

  get id () {
    return this.state[3]
  }

  get read () {
    return this.state[1]
  }

  get written () {
    return this.state[2]
  }

  set written (bytes) {
    this.state[2] = bytes
  }

  set read (bytes) {
    this.state[1] = bytes
  }

  slice (start = 0, end = this.size) {
    return this.u8.subarray(start, end)
  }
}

spin.RawBuffer = RawBuffer

const _load = spin.load
let _system

function getSystem () {
  if (_system) return _system
  const lib = _load('system')
  if (!lib) return
  const { system } = lib
  const _dlopen = system.dlopen
  system.dlopen = (path = '') => _dlopen(CString(path).ptr, 1)
  const _dlsym = system.dlsym
  system.dlsym = (handle, sym = '') => _dlsym(handle, CString(sym).ptr)
  _system = system
  return system
}

spin.load = name => {
  const lib = _load(name)
  if (lib) return lib
  const system = getSystem()
  if (!system) return
  const handle = system.dlopen(`module/${name}/${name}.so`)
  if (!handle) return
  const sym = system.dlsym(handle, `_register_${name}`)
  if (!sym) return
  return _load(BigInt(sym))
}

async function main () {
  try {
    const { main, serve, test } = await import(spin.args[1])
    if (test) {
      await test(...spin.args.slice(2))
    }
    if (main) {
      await main(...spin.args.slice(2))
    }
    if (serve) {
      await serve(...spin.args.slice(2))
    }
  } catch (err) {
    console.error(`${AR}${err.message}${AD}\n${err.stack}`)
  }
}

if (spin.args.length > 1) {
  if (spin.args[1] === 'gen') {
    const { bindings, linkerScript, headerFile } = await import('lib/gen.js')
    let source = ''
    if (spin.args[2] === '--link') {
      source += await linkerScript('main.js')
      for (const fileName of spin.args.slice(3)) {
        source += await linkerScript(fileName)
      }
    } else if (spin.args[2] === '--header') {
      source = await headerFile(spin.args.slice(3))
    } else {
      source = await bindings(spin.args[2])
    }
    console.log(source)  
  } else {
    await main(...spin.args.slice(1))
  }
} else {
  const versions = spin.version
  console.log(`spin ${versions.spin} v8 ${versions.v8}`)
}
