import { system } from 'lib/system.js'

const { getrlimit, setrlimit, constants, hrtime } = system
const { RLIMIT_NOFILE } = constants

function benchget (count) {
  const start = hrtime()
  for (let i = 0; i < count; i++) getrlimit(RLIMIT_NOFILE)
  const elapsed = (hrtime() - start) / 1000000
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${Math.floor(elapsed)} ms rate ${rate} (getrlimit)`)
}

function benchset (count) {
  const start = hrtime()
  for (let i = 0; i < count; i++) setrlimit(RLIMIT_NOFILE, maxFiles, maxFiles)
  const elapsed = (hrtime() - start) / 1000000
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${Math.floor(elapsed)} ms rate ${rate} (setrlimit)`)
}

function run (fn, count, repeat = 10) {
  for (let i = 0; i < repeat; i++) fn(count)
}

const repeat = Number(spin.args[2] || 10)
const maxFiles = 1 * 1024 * 1024

spin.assert(getrlimit(RLIMIT_NOFILE)[0] === maxFiles)
spin.assert(setrlimit(RLIMIT_NOFILE, maxFiles / 2, maxFiles) === 0)
spin.assert(getrlimit(RLIMIT_NOFILE)[0] === (maxFiles / 2))
spin.assert(getrlimit(RLIMIT_NOFILE)[1] === maxFiles)

while (1) {
  run(benchget, 3000000, repeat)
  run(benchset, 3000000, repeat)
}
