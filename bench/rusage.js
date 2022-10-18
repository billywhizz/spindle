const { system } = spin.load('system')

const rusage = new ArrayBuffer(148)
const u32 = new Uint32Array(rusage)
const ptr = spin.getAddress(rusage)

const repeat = 10
const count = 1000000

function bench () {
  const start = Date.now()
  for (let i = 0; i < count; i++) maxrss()
  const elapsed = Date.now() - start
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${elapsed} ms rate ${rate}`)
}

function maxrss () {
  system.getrusage(0, ptr)
  return u32[8]
}

console.log(maxrss())

for (let i = 0; i < repeat; i++) bench()