const { loop } = spin.load('loop')

const { create, wait, modify, close } = loop

const EPOLLIN = 0x1
const EPOLLOUT = 0x4
const EPOLLERR = 0x8
const EPOLLHUP = 0x10
const EPOLL_CLOEXEC = 524288
const EPOLLEXCLUSIVE = 1 << 28
const EPOLLWAKEUP = 1 << 29
const EPOLLONESHOT = 1 << 30
const EPOLLET = 1 << 31
const EPOLL_CTL_ADD = 1
const EPOLL_CTL_DEL = 2
const EPOLL_CTL_MOD = 3
const EVENT_SIZE = 12

loop.constants = {
  EPOLLIN, EPOLLOUT, EPOLLERR, EPOLLHUP, EPOLL_CLOEXEC, EPOLLEXCLUSIVE,
  EPOLLWAKEUP, EPOLLONESHOT, EPOLLET, EPOLL_CTL_ADD, EPOLL_CTL_DEL,
  EPOLL_CTL_MOD
}

function event (fd, mask = EPOLLIN | EPOLLOUT) {
  const buf = new ArrayBuffer(EVENT_SIZE)
  const dv = new DataView(buf)
  dv.setUint32(0, mask, true)
  dv.setUint32(4, fd, true)
  const u8 = new Uint8Array(buf)
  u8.ptr = spin.getAddress(buf)
  return u8
}

function events (nevents = 1024) {
  const buf = new ArrayBuffer(nevents * EVENT_SIZE)
  const u32 = new Uint32Array(buf)
  u32.ptr = spin.getAddress(buf)
  return u32
}

loop.types = {
  event,
  events
}

const noop = () => {}

class Loop {
  constructor (nevents = 1024, flags = EPOLL_CLOEXEC) {
    this.maxEvents = nevents
    this.events = events(nevents)
    this.fd = create(flags)
    this.handles = {}
  }

  add (fd, callback, onerror = noop) {
    const ev = event(fd, EPOLLIN)
    const rc = modify(this.fd, EPOLL_CTL_ADD, fd, ev.ptr)
    if (rc === -1) return rc
    this.handles[fd] = { event: ev, callback, onerror }
  }

  remove (fd) {
    return modify(this.fd, EPOLL_CTL_DEL, fd, 0)
  }

  poll (timeout = -1) {
    const { fd, maxEvents, events, handles } = this
    const n = wait(fd, events.ptr, maxEvents, timeout)
    let off = 0
    for (let i = 0; i < n; i++) {
      const mask = events[off++]
      const fd = events[off++]
      if (mask & EPOLLERR || mask & EPOLLHUP) {
        close(fd)
        handles[fd] && handles[fd].onerror(fd)
        delete handles[fd]
        off++
        continue
      }
      handles[fd] && handles[fd].callback(fd)
      off++
    }
    return n
  }
}

export { loop, Loop }
