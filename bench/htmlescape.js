import { system } from 'lib/system.js'
import { pico } from 'lib/pico.js'

const { escapeHTML } = pico
const { hrtime } = system

var ma = /["'&<>]/;
const Q = "&quot;"
const A = "&amp;"
const P = "&#x27;"
const L = "&lt;"
const G = "&gt;"

function HTMLEscape(a) {
  var b = ma.exec(a)
  if (b) {
    var c = '', d, f = 0
    for (d = b.index; d < a.length; d++) {
      switch (a.charCodeAt(d)) {
        case 34:
          b = Q
          break
        case 38:
          b = A
          break
        case 39:
          b = P
          break
        case 60:
          b = L
          break
        case 62:
          b = G
          break
        default:
          continue
      }
      f !== d && (c += a.substring(f, d))
      f = d + 1
      c += b
    }
    a = f !== d ? c + a.substring(f, d) : c
  }
  return a
}

function benchSpindle (count) {
  const start = hrtime()
  for (let i = 0; i < count; i++) escapeSpindle()
  const elapsed = (hrtime() - start) / 1000000
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${Math.floor(elapsed)} ms rate ${rate}`)
}

function benchJS (count) {
  const start = hrtime()
  for (let i = 0; i < count; i++) escapeJS()
  const elapsed = (hrtime() - start) / 1000000
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${Math.floor(elapsed)} ms rate ${rate}`)
}

const expected = /&lt;script&gt;alert\(&quot;This should not be displayed \d{16} in a browser alert box.&quot;\);&lt;\/script&gt;/
const str = '<script>alert("This should not be displayed 0 in a browser alert box.");</script>'

const payloads = []
const spayloads = []
const idx = new Uint16Array(1)
for (let i = 0; i < Math.pow(2, 16); i++) {
  payloads.push(spin.CString(str.replace('0', Math.floor(Math.random() * Math.pow(2, 31)).toString().padStart(16, '0'))))
  spayloads.push(str.replace('0', Math.floor(Math.random() * Math.pow(2, 31)).toString().padStart(16, '0')))
}

const out = new spin.RawBuffer(4096)

function escapeSpindle () {
  const payload = payloads[idx[0]++]
  out.written = escapeHTML(out.ptr, payload.ptr, payload.size)
  return spin.readLatin1(out.id)
}

function escapeJS () {
  return HTMLEscape(spayloads[idx[0]++])
}

let output = escapeSpindle()
console.log(output)
spin.assert(output.match(expected))
output = escapeJS(str)
spin.assert(output.match(expected))

for (let i = 0; i < 5; i++) {
  benchSpindle(10000000)
  benchJS(10000000)
}
