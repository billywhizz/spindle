import { pico } from 'lib/pico.js'

const src = new spin.RawBuffer(65536)
const dest = new spin.RawBuffer(src.size)

spin.writeLatin1(src.id, '0123456789'.repeat(1024))
const written = pico.zlibDeflate(src.ptr, src.written, dest.ptr, dest.size)
spin.assert(written === 104)

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