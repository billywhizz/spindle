import { system } from 'lib/system.js'
const { fs } = spin.load('fs')

const { hrtime } = system

const O_RDONLY = 0
const SEEK_SET = 0

const procFileName = spin.CString(`/proc/meminfo`)
const fd = fs.open(procFileName.ptr, O_RDONLY)
const buf = new spin.RawBuffer(1024)

const { ptr, size } = buf
const { read, lseek } = fs

function check () {
  lseek(fd, 0, SEEK_SET)
  buf.written = read(fd, ptr, size)
}

function bench (count) {
  const start = hrtime()
  for (let i = 0; i < count; i++) check()
  const elapsed = (hrtime() - start) / 1000000
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${Math.floor(elapsed)} ms rate ${rate}`)
}

for (let i = 0; i < 5; i++) {
  bench(1000000)
}
