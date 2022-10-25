const { net } = spin.load('net')
const { fs } = spin.load('fs')

import { system } from 'lib/system.js'
import { Loop } from 'lib/loop.js'

function CString (str) {
  const buf = spin.calloc(1, `${str}\0`)
  buf.ptr = spin.getAddress(buf)
  return buf
}

const status = new Int32Array(1)
status.ptr = spin.getAddress(status.buffer)
const WNOHANG = 1

function waitpid (pid) {
  return system.waitpid(pid, status.ptr, WNOHANG)
}

function createArgs (...args) {
  const command = CString(args[0])
  const len = args.length
  const buf = new ArrayBuffer((len + 1) * 8)
  const pointers = new BigUint64Array(buf)
  buf.ptr = spin.getAddress(buf)
  pointers[0] = BigInt(command.ptr)
  const cargs = []
  for (let i = 1; i < len; i++) {
    const carg = CString(args[i])
    cargs.push(carg)
    pointers[i] = BigInt(carg.ptr)
  }
  return { command, args: buf, cargs }
}

class Process {
  constructor (...args) {
    this.command = args[0]
    this.args = args.slice(1)
  }

  spawn () {
    const pid = system.fork()
    if (pid === -1) throw new Error('Could not fork')
    if (pid === 0) {
      const { command, args, cargs } = createArgs(this.command, ...this.args)
      const devnull = fs.open(CString('/dev/null').ptr, O_WRONLY)
      net.dup2(devnull, 1)
      net.dup2(devnull, 2)
      this.foo = { command, args, cargs }
      system.execvp(command.ptr, args.ptr)
      system.exit(127)
    } else {
      this.pid = pid
      this.procFileName = CString(`/proc/${pid}/stat`)
      this.procfd = fs.open(this.procFileName.ptr, O_RDONLY)
      this.fd = system.pidfd_open(pid)
      if (this.fd < 1) {
        console.log(`${system.errno} : ${system.strerror(system.errno)}`)
      }
      this.buf = new spin.RawBuffer(1024)
    }
    return this
  }

  get stat () {
    const { procfd, buf } = this
    fs.lseek(procfd, 0, SEEK_SET)
    const bytes = fs.read(procfd, buf.ptr, buf.size)
    if (bytes <= 0) console.log(bytes)
    buf.state[2] = bytes
    return spin.readLatin1(buf.id).trim()
  }
}

function onProcessComplete (fd) {
  eventLoop.remove(fd)
  net.close(fd)
  const process = processes[fd]
  net.close(process.procfd)
  const rc = waitpid(process.pid)
  addCompletion(process.pid, status[0])
  //console.log(`process.exit ${process.pid} exit ${status[0]} wait ${rc}`)
  delete processes[fd]
  deaths++
  spawn('/bin/sleep', (Math.ceil(Math.random() * sleepTime) + sleepTime).toString())
  if (!Object.keys(processes).length) {
    console.log('stopping timer')
    eventLoop.remove(tfd)
    net.close(tfd)
  }
}

function spawn (...args) {
  const process = (new Process(...args)).spawn()
  eventLoop.add(process.fd, onProcessComplete)
  processes[process.fd] = process
  //const rc = waitpid(process.pid)
  //console.log(`process.start ${process.pid} exit ${status[0]} wait ${rc}`)
  return process
}

const O_RDONLY = 0
const O_WRONLY = 1
const SEEK_SET = 0

const processes = {}
const completions = {}

function addCompletion (pid, status) {
  if (!completions[status]) {
    completions[status] = 1
    return
  }
  completions[status]++
}

const eventLoop = new Loop()
const tfd = system.timer(1000, 1000)
const tbuf = new Uint8Array(8)
tbuf.ptr = spin.getAddress(tbuf.buffer)

let deaths = 0

eventLoop.add(tfd, () => {
  net.read(tfd, tbuf.ptr, 8)
  const start = system.hrtime()
  const pids = Object.keys(processes)
  for (const fd of pids) {
    const process = processes[fd]
    const { stat } = process
    //console.log(stat)
  }
  const [utime, stime, , , ticks] = system.cputime()
  const usr = ((utime - lastUsr))
  const sys = ((stime - lastSys))
  lastUsr = utime
  lastSys = stime
  lastTicks = ticks
  const total = usr + sys
  const [rss] = system.getrusage()
  const elapsed = (system.hrtime() - start) / 1000000
  const ckeys = Object.keys(completions)
  console.log(`rss ${rss} proc ${Object.keys(processes).length} time ${elapsed} ms usr ${usr} sys ${sys} total ${total} (${ckeys.map(k => `${k} : ${completions[k]}`)}) deaths ${deaths} loop ${Math.floor((start - lastLoop) / 1000000)}`)
  lastLoop = system.hrtime()
  deaths = 0
})

eventLoop.tbuf = tbuf

let [lastUsr, lastSys, , , lastTicks] = system.cputime()
const maxFiles = 1 * 1024 * 1024
const rc = system.setrlimit(system.constants.RLIMIT_NOFILE, maxFiles, maxFiles)
if (rc !== 0) console.error(system.strerror(system.errno))

const numProcs = parseInt(spin.args[2] || '10')
const sleepTime = parseInt(spin.args[3] || '30')
for (let i = 0; i < numProcs; i++) {
  spawn('/bin/sleep', (Math.ceil(Math.random() * sleepTime) + sleepTime).toString())
}

let lastLoop = system.hrtime()
while (1) {
  spin.runMicroTasks()
  if (eventLoop.size === 0) break
  if (eventLoop.poll(-1) === 0) break
}
spin.runMicroTasks()

