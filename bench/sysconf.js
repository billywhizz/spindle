import { system } from 'lib/system.js'

const { sysconf } = system

const _SC_AVPHYS_PAGES = 86

function meminfo () {
  return sysconf(_SC_AVPHYS_PAGES)
}

function benchSysconf (count) {
  const start = Date.now()
  for (let i = 0; i < count; i++) meminfo()
  const elapsed = Date.now() - start
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${elapsed} ms rate ${rate}`)
}

function run (fn, count, repeat = 10) {
  for (let i = 0; i < repeat; i++) fn(count)
}

const repeat = Number(spin.args[2] || 10)

while (1) {
  run(benchSysconf, 3000000, repeat)
}
