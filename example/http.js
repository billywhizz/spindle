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
const { SystemError } = system

const HTTP_CTX_SZ = 32
const HTTP_HEADER_SZ = 32
const MAXHEADERS = 14

function httpState () {
  const u8 = new Uint8Array(HTTP_CTX_SZ + (HTTP_HEADER_SZ * MAXHEADERS))
  const u32 = new Uint32Array(u8.buffer)
  u32.ptr = spin.getAddress(u8.buffer)
  return u32
}

class Response {
  constructor (str = '') {
    this.body = str
  }
}

class Request {
  #path_len = 0
  #method_len = 0
  #num_headers = 0
  minorVersion = 1
  #request = ''
  #method = ''
  #path = ''
  #headers = null

  constructor (state, text) {
    [this.#path_len, , this.#method_len, , this.#num_headers, , this.minorVersion] = state
    this.#request = text
  }

  get method () {
    if (this.#method) return this.#method
    this.#method = this.#request.slice(0, this.#method_len)
    return this.#method
  }

  get path () {
    if (this.#path) return this.#path
    this.#path = this.#request.slice(this.#method_len + 1, this.#method_len + 1 + this.#path_len)
    return this.#path
  }

  get headers () {
    if (this.#headers) return this.#headers
    const [, ...headers] = this.#request.split('\r\n')
    this.#headers = headers
    return this.#headers
  }
}

class Server {
  #port = 3000
  #address = '127.0.0.1'
  fetch = noop
  mode = modes.Sync
  nargs = 0
  fd = 0
  sockets = {}
  loop = null

  constructor (port = 3000, address = '127.0.0.1') {
    this.#port = port
    this.#address = address
    this.fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)
  }

  get port () {
    return this.#port
  }

  get address () {
    return this.#address
  }

  onConnect (fd) {
    const { sockets, loop } = this
    const cfd = accept4(fd, 0, 0, O_NONBLOCK)
    if (cfd > 0) {
      loop.add(cfd, () => {
        sockets[cfd] = new HttpSocket(cfd, this.fetch, loop, this.nargs)
        sockets[cfd].loop = loop
      }, Loop.Writable)
      return
    }
    if (system.errno === EAGAIN) return
    close(cfd)
  }

  listen (backlog = SOMAXCONN, reusePort = true) {
    const tfd = system.timer(1000, 1000)
    const tbuf = new spin.RawBuffer(8)
    this.loop.add(tfd, () => {
      console.log(`${system.getrusage()[0]}`)
      net.read(tfd, tbuf.ptr, 8)
    })
    this.tfd = tfd
    this.tbuf = tbuf
    const { fd, port, address } = this
    if (reusePort) setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, net.on, 32)
    spin.assert(bind(fd, sockaddr_in(address, port).address, 
      SOCKADDR_LEN) === 0, 'bind', SystemError)
    spin.assert(listen(fd, backlog) === 0, 'listen', SystemError)
  }

  close () {
    close(this.tfd)
  }
}

const noop = () => {}

class HttpSocket {
  fd = 0
  #handler
  #buf
  #state = httpState()
  loop
  #nargs = 0

  constructor (fd, handler, loop, nargs = 0, size = 4096) {
    this.fd = fd
    this.#handler = handler
    this.#buf = new spin.RawBuffer(size)
    this.loop = loop
    this.#nargs = nargs
    loop.modify(fd, Loop.Readable, fd => this.onReadable(fd))
  }

  onReadable (fd) {
    const buf = this.#buf
    const state = this.#state
    const handler = this.#handler
    const nargs = this.#nargs
    const bytes = recv(fd, buf.ptr, buf.size, 0)
    if (bytes > 0) {
      const nread = pico.parseRequest(buf.ptr, bytes, state.ptr)
      if (nread === bytes) {
        buf.written = nread - 4
        let res
        if (nargs === 0) {
          res = handler()
        } else if (nargs === 1) {
          res = handler(new Request(state, spin.readLatin1(buf.id)))
        }
        // todo: c++ respond method
        spin.writeLatin1(sendbuf.id, `HTTP/1.1 200 OK\r\nServer: j\r\nDate: Thu, 03 Nov 2022 00:02:50 GMT\r\nContent-Type: text/plain,charset=utf8\r\nContent-Length: 13\r\n\r\n${res.body}`)
        send(fd, sendbuf.ptr, sendbuf.written, 0)
        return
      }
      console.log(`parse ${nread}`)
    }
    if (bytes < 0 && system.errno === EAGAIN) return
    if (bytes < 0) console.error('socket_error')
    this.loop.remove(fd)
    close(fd)
  }
}


const sendbuf = new spin.RawBuffer(4096)

const modes = {
  Async: 0,
  Sync: 1
}

function serve (config) {
  const { address = '127.0.0.1', port = 3000, fetch } = config
  const server = new Server(port, address)
  const eventLoop = new Loop()
  server.mode = fetch.constructor.name === 'AsyncFunction' ? modes.Async: modes.Sync
  server.args = fetch.length
  server.loop = eventLoop
  server.fetch = fetch
  eventLoop.add(server.fd, fd => server.onConnect(fd))
  server.listen()
  console.log(`listening on ${server.address}:${server.port} with ${server.mode} handler with ${server.nargs} args`)
  while (1) {
    //spin.runMicroTasks()
    if (eventLoop.poll(-1) === 0) break
  }
}

export { serve, Response, Request }
