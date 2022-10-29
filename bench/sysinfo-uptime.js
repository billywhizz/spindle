import { system } from 'lib/system.js'

const { uptime, hrtime } = system

function benchUptime (count) {
  const start = hrtime()
  for (let i = 0; i < count; i++) uptime()
  const elapsed = (hrtime() - start) / 1000000
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${Math.floor(elapsed)} ms rate ${rate}`)
}

function run (fn, count, repeat = 10) {
  for (let i = 0; i < repeat; i++) fn(count)
}

const repeat = Number(spin.args[2] || 10)

const seconds = uptime()
const minute = 60
const hour = minute * 60
const day = hour * 24
const updays = Math.floor(seconds / day)
const uphours = Math.floor(seconds / hour) - (updays * 24)
const upmins = Math.floor(seconds / minute) - (uphours * 60)
console.log(`${updays} d ${uphours} h ${upmins} m`)

while (1) {
  run(benchUptime, 3000000, repeat)
}
