const { system } = spin.load('system')

const timespec = new Uint8Array(16)
const t32 = new Uint32Array(timespec.buffer)
const tptr = spin.getAddress(timespec.buffer)

const CLOCK_REALTIME = 0
const CLOCK_MONOTONIC = 1
const TFD_NONBLOCK = 2048
const TFD_CLOEXEC = 524288

const { clock_gettime } = system

function hrtime () {
  clock_gettime(CLOCK_MONOTONIC, tptr)
  return (t32[0] * 1e9) + t32[2]
}

const repeat = 10
const count = Number(spin.args[3] || 1000000)

function bench () {
  const start = Date.now()
  for (let i = 0; i < count; i++) hrtime()
  const elapsed = Date.now() - start
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${elapsed} ms rate ${rate}`)
}

for (let i = 0; i < repeat; i++) bench()