import { system } from './lib/system.js'

const { getrusage, hrtime } = system

function benchrUsage (count) {
  const start = Date.now()
  for (let i = 0; i < count; i++) getrusage()
  const elapsed = Date.now() - start
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${elapsed} ms rate ${rate}`)
}

function benchHRTime (count) {
  const start = Date.now()
  for (let i = 0; i < count; i++) hrtime()
  const elapsed = Date.now() - start
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${elapsed} ms rate ${rate}`)
}

function run (fn, count, repeat = 10) {
  for (let i = 0; i < repeat; i++) fn(count)
}

const repeat = Number(spin.args[2] || 10)

while (1) {
  run(benchHRTime, 100000000, repeat)
  run(benchrUsage, 3000000, repeat)
}
