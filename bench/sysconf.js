import { system } from 'lib/system.js'

const { sysconf, hrtime } = system

const _SC_AVPHYS_PAGES = 86

function meminfo () {
  return sysconf(_SC_AVPHYS_PAGES)
}

function benchSysconf (count) {
  const start = hrtime()
  for (let i = 0; i < count; i++) meminfo()
  const elapsed = (hrtime() - start) / 1000000
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${Math.floor(elapsed)} ms rate ${rate}`)
}

function run (fn, count, repeat = 10) {
  for (let i = 0; i < repeat; i++) fn(count)
}

const repeat = Number(spin.args[2] || 10)

while (1) {
  run(benchSysconf, 3000000, repeat)
}
