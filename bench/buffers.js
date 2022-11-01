import { system } from 'lib/system.js'

const { hrtime } = system

function bench (count) {
  const start = hrtime()
  for (let i = 0; i < count; i++) createBuffer()
  const elapsed = (hrtime() - start) / 1000000
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${Math.floor(elapsed)} ms rate ${rate}`)
}

function run (fn, count, repeat = 10) {
  for (let i = 0; i < repeat; i++) fn(count)
}

const repeat = Number(spin.args[2] || 10)

function createBuffer () {
  return new ArrayBuffer(4096)
}

while (1) {
  run(bench, 1000000, repeat)
}
