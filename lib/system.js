const { system } = spin.load('system')

const nullstring = '\0'

const _dlopen = system.dlopen
system.dlopen = (path = nullstring) => {
  return _dlopen(spin.getAddress(spin.calloc(1, path)), 1)
}

const _dlsym = system.dlsym
system.dlsym = (handle, sym = nullstring) => {
  return _dlsym(handle, spin.getAddress(spin.calloc(1, sym)))
}

const handle = system.dlopen()
const errnoPtr = system.dlsym(handle, 'errno')
const errnob = new Uint32Array(spin.readMemory(BigInt(errnoPtr), BigInt(errnoPtr) + 4n))

Object.defineProperty(system, 'errno', {
  configurable: false,
  enumerable: true,
  get: () => errnob[0]
})

system.constants = {
  EAGAIN: 11,
  MAX_PATH: 4096,
  CLOCK_REALTIME: 0,
  CLOCK_MONOTONIC: 1,
  TFD_NONBLOCK: 2048,
  TFD_CLOEXEC: 524288,
  RUSAGE_SELF: 0,
  RUSAGE_THREAD: 1
}

const rbuf = new ArrayBuffer(148)
const u32 = new Uint32Array(rbuf)
const rptr = spin.getAddress(rbuf)
const stats = u32.subarray(8)

const _getrusage = system.getrusage
system.getrusage = (flags = system.constants.RUSAGE_SELF) => {
  _getrusage(flags, rptr)
  return stats
}

const timespec = new Uint8Array(16)
const t32 = new Uint32Array(timespec.buffer)
const tptr = spin.getAddress(timespec.buffer)

const { clock_gettime } = system
const { CLOCK_MONOTONIC } = system.constants

system.hrtime = () => {
  clock_gettime(CLOCK_MONOTONIC, tptr)
  return (t32[0] * 1e9) + t32[2]
}

export { system }
