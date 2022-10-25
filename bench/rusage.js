import { system } from 'lib/system.js'

const { getrusage, hrtime } = system

function bench (count) {
  const start = hrtime()
  for (let i = 0; i < count; i++) getrusage()
  const elapsed = (hrtime() - start) / 1000000
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${Math.floor(elapsed)} ms rate ${rate}`)
}

function run (fn, count, repeat = 10) {
  for (let i = 0; i < repeat; i++) fn(count)
}

const repeat = Number(spin.args[2] || 10)

while (1) {
  run(bench, 3000000, repeat)
}
