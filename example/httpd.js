import { net } from 'lib/net.js'
import { Loop } from 'lib/loop.js'
import { system } from 'lib/system.js'
import { pico } from 'lib/pico.js'

const { socket, setsockopt, bind, listen, close, accept4, send, recv } = net
const { 
  SOCK_STREAM, AF_INET, SOCK_NONBLOCK, SOL_SOCKET, SO_REUSEPORT, SOCKADDR_LEN, 
  SOMAXCONN, O_NONBLOCK
} = net.constants
const { sockaddr_in } = net.types
const { EAGAIN } = system.constants

function httpState () {
  const u8 = new Uint8Array(HTTP_CTX_SZ + (HTTP_HEADER_SZ * MAXHEADERS))
  const u32 = new Uint32Array(u8.buffer)
  u32.ptr = spin.getAddress(u8.buffer)
  return u32
}

function onSocketEvent (fd) {
  const bytes = recv(fd, buf.ptr, BUFSIZE, 0)
  if (bytes > 0) {
    const nread = pico.parseRequest(buf.ptr, bytes, info)
    if (nread !== bytes) console.log(`${nread}:${bytes}`)
    send(fd, sPtr, size, 0)
    return
  }
  if (bytes < 0 && system.errno === EAGAIN) return
  if (bytes < 0) console.error('socket_error')
  eventLoop.remove(fd)
  close(fd)
}

function onConnect (fd) {
  const newfd = accept4(fd, 0, 0, O_NONBLOCK)
  if (newfd > 0) {
    eventLoop.add(newfd, onSocketEvent)
    return
  }
  if (system.errno === EAGAIN) return
  close(newfd)
}

const HTTP_CTX_SZ = 32
const HTTP_HEADER_SZ = 32
const MAXHEADERS = 14
const BUFSIZE = 16384
const buf = new spin.RawBuffer(BUFSIZE)
const state = httpState()
const info = state.ptr
const sendbuf = spin.calloc(1, `HTTP/1.1 200 OK\r\nContent-Type: text/plain,charset=utf8\r\nContent-Length: 13\r\n\r\nHello, World!`)
const sPtr = spin.getAddress(sendbuf)
const size = sendbuf.byteLength
const eventLoop = new Loop()
const address = '127.0.0.1'
const port = 3000
const sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)
setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, net.on, 32)
bind(sfd, sockaddr_in(address, port).address, SOCKADDR_LEN)
listen(sfd, SOMAXCONN)
eventLoop.add(sfd, onConnect)
while (1) {
  //spin.runMicroTasks()
  if (eventLoop.poll(-1) === 0) break
}
