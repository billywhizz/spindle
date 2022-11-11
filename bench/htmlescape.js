import { system } from 'lib/system.js'

const { hrtime } = system
const { assert } = spin

const { html } = spin.load('html')

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

function bench (count) {
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

const bin = new ArrayBuffer(4096)
const bout = new ArrayBuffer(4096)
const escaper = html.escaper(bin, bout)

function escapeSpindle () {
  return escaper.escape(payloads[idx[0]++])
}

function escapeJS () {
  return HTMLEscape(payloads[idx[0]++])
}

const payloads = new Array(Math.pow(2, 16)).fill('')
const idx = new Uint16Array(1)
for (let i = 0; i < Math.pow(2, 16); i++) {
  const num = Math.floor(Math.random() * Math.pow(2, 31)).toString().padStart(16, '0')
  payloads[i] = str.replace('0', num)
  const escaped = escaper.escape(payloads[i])
  assert(escaped.match(expected))
  assert(escaped.indexOf(num) > 0)
}

for (let i = 0; i < 100; i++) {
  bench(10000000)
  benchJS(3200000)
}
