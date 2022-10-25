import { system } from 'lib/system.js'
import { net } from 'lib/net.js'
import { Loop } from 'lib/loop.js'

const { timer } = system
const { test } = spin.load(BigInt(system.dlsym(system.dlopen('./modules/test/test.so'), '_register_test')))
const { AY, AG, AD, AC, AR } = spin.colors
const rx = /\B(?=(\d{3})+(?!\d))/g
const comma = ","
let running = true

function numberWithCommas(x) {
  return x.toString().replace(rx, comma)
}

function format (num, len = 10) {
  return numberWithCommas(num).padStart(len, ' ')
}

function formatFloat (num, len = 10, places = 3) {
  return (Math.floor(num * 1000) / 1000).toFixed(places).padStart(len, ' ')
}

function onTimer () {
  const [utime, stime, , , ticks] = system.cputime()
  const usr = ((utime - lastUsr))
  const sys = ((stime - lastSys))
  lastUsr = utime
  lastSys = stime
  lastTicks = ticks
  const total = usr + sys
  console.log(`poll${id},${nevents},${cbs},${ops},${polls},${system.getrusage()[0]},${usr},${sys},${total}`)
  //console.log(`${AG}${`poll${id}`.padEnd(5, ' ')}${AD} ${format(nevents, 8)} ${AY}size${AD} ${format(cbs, 12)} ${AY}callbacks${AD} ${format(ops, 12)} ${AY}ops${AD} ${format(polls, 12)} ${AY}polls${AD} ${format(system.getrusage()[0], 9)} ${AC}rss${AD} ${formatFloat(usr, 6, 2)} ${AG}usr${AD} ${formatFloat(sys, 6, 2)} ${AR}sys${AD} ${formatFloat(total, 6, 2)} ${AY}total${AD}`)
  cbs = 0
  polls = 0
  ops = 0
  net.read(tfd, tb.ptr, tb.size)
  if (!--seconds) running = false
}

const eventLoop = new Loop()
const tfd = timer(1000, 1000)
const tb = new spin.RawBuffer(8)
let cbs = 0
let polls = 0
let ops = 0
const promises = []
let [lastUsr, lastSys, , , lastTicks] = system.cputime()

const resolvers = {}
resolvers.opresolve1 = (...args) => {
  cbs++
  for (let i = 0; i < args.length; i += 2) promises[args[i]].resolve(args[i + 1])
}
resolvers.opresolve2 = res => {
  cbs++
  ops++
}
resolvers.opresolve3 = (...args) => {
  cbs++
  for (let i = 0; i < args.length; i++) promises[i32[i]].resolve(args[i])
}
resolvers.opresolve4 = (n) => {
  cbs++
  for (let i = 0; i < n; i++) promises[i32[i]].resolve(objs[i])
}

const nevents = parseInt(spin.args[2] || 32)
const i32 = new Int32Array(nevents)
const id = parseInt(spin.args[3] || '1', 10)
let seconds = parseInt(spin.args[4] || '5', 10)
for (let i = 0; i < nevents; i++) promises.push({ resolve: () => { ops++ } })
eventLoop.add(tfd, onTimer)
const objs = new Array(nevents).fill(1)

let resolver = resolvers[`opresolve${id}`]
let poller = test[`poll${id}`]
test.register(resolver, i32, objs)
while (running) {
  spin.runMicroTasks()
  poller()
  polls++
  eventLoop.poll(0)
  if (!eventLoop.size) break
}
