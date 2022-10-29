import { net } from 'lib/net.js'
import { Loop } from 'lib/loop.js'
import { system } from 'lib/system.js'

const { timer } = system
const eventLoop = new Loop()

const tfd = timer(1000, 1000)
const tbuf = new Uint8Array(8)
tbuf.ptr = spin.getAddress(tbuf.buffer)
eventLoop.add(tfd, () => {
  console.log(`${system.getrusage()[0]}`)
  net.read(tfd, tbuf.ptr, 8)
})

while (1) {
  spin.runMicroTasks()
  if (eventLoop.poll(-1) === 0) break
}
