import { system } from 'lib/system.js'
import { pico } from 'lib/pico.js'

const { escapeHTML } = pico
const { hrtime } = system

function bench (count) {
  const start = hrtime()
  for (let i = 0; i < count; i++) escape()
  const elapsed = (hrtime() - start) / 1000000
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${Math.floor(elapsed)} ms rate ${rate}`)
}

const str = '<script type=\'\' src="">const s = "국제 회의가"</script>'
const payload = spin.calloc(1, str)
payload.ptr = spin.getAddress(payload)
const out = new spin.RawBuffer(4096)
const len = payload.byteLength

function escape () {
  return escapeHTML(out.ptr, payload.ptr, len)
}

console.log(spin.readUtf8(out.id, escape()))

for (let i = 0; i < 5; i++) {
  bench(10000000)
}
