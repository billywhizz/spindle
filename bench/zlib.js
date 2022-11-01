import { system } from 'lib/system.js'
import { pico } from 'lib/pico.js'

const { hrtime } = system

const src = new spin.RawBuffer(65536)
const dest = new spin.RawBuffer(src.size)

spin.writeLatin1(src.id, '0123456789'.repeat(1024))
const written = pico.zlibDeflate(src.ptr, src.written, dest.ptr, dest.size)
spin.assert(written === 104)

const { zlibDeflate } = pico

function deflate () {
  zlibDeflate(src.ptr, src.written, dest.ptr, dest.size)
}

function bench (count) {
  const start = hrtime()
  for (let i = 0; i < count; i++) deflate()
  const elapsed = (hrtime() - start) / 1000000
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${Math.floor(elapsed)} ms rate ${rate}`)
}

const repeat = Number(spin.args[2] || 10)
const count = Number(spin.args[3] || 100000)

for (let i = 0; i < repeat; i++) bench(count)