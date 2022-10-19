import { system } from './lib/system.js'

const { getrusage, hrtime } = system

function bench (fn, count) {
  const start = Date.now()
  for (let i = 0; i < count; i++) fn()
  const elapsed = Date.now() - start
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${elapsed} ms rate ${rate}`)
}

function run (fn, count, repeat = 10) {
  for (let i = 0; i < repeat; i++) bench(fn, count)
}

run(hrtime, 100000000)
run(getrusage, 1000000)
