import { pico } from 'lib/pico.js'
import { dump } from 'scratch/pretty.js'

class RawBuffer {
  constructor (size) {
    const buf = new ArrayBuffer(size)
    this.ptr = spin.getAddress(buf)
    this.state = new Uint32Array(6)
    this.u8 = new Uint8Array(buf)
    spin.rawBuffer(buf, this.state.buffer)
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

  slice (start = 0, end = this.size) {
    return this.u8.subarray(start, end)
  }
}

const src = new RawBuffer(65536)
const dest = new RawBuffer(src.size)

spin.writeLatin1(src.id, '0123456789'.repeat(1024))
const written = pico.zlibDeflate(src.ptr, src.written, dest.ptr, dest.size)
console.log(written)
console.log(dump(dest.slice(0, written), written, 0, 16, 0, false))

function bench (count) {
  const start = Date.now()
  for (let i = 0; i < count; i++) pico.zlibDeflate(src.ptr, src.written, dest.ptr, dest.size)
  const elapsed = Date.now() - start
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${elapsed} ms rate ${rate}`)
}

const repeat = Number(spin.args[2] || 10)
const count = Number(spin.args[3] || 10000)

for (let i = 0; i < repeat; i++) bench(count)