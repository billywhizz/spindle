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

const modes = {
  Async: 0,
  Sync: 1
}

const statusMessages = {
  200: 'OK',
  201: 'Created',
  204: 'OK',
  206: 'Partial Content',
  101: 'Switching Protocols',
  302: 'Redirect',
  400: 'Bad Request',
  401: 'Unauthorized',
  403: 'Forbidden',
  404: 'Not Found',
  429: 'Server Busy',
  500: 'Server Error'
}

const contentTypes = {
  text: 'text/plain',
  css: 'text/css',
  utf8: 'text/plain;charset=utf-8',
  json: 'application/json;charset=utf-8',
  html: 'text/html;charset=utf-8',
  octet: 'application/octet-stream',
  ico: 'application/favicon',
  png: 'application/png',
  xml: 'application/xml;charset=utf-8',
  js: 'application/javascript;charset=utf-8',
  wasm: 'application/wasm'
}

class Response {
  body
  status = 200
  contentType = 'utf8'
  chunked = false
  fd = 0
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

function serve (fetch, config = {}) {
  function httpState () {
    const u8 = new Uint8Array(HTTP_CTX_SZ + (HTTP_HEADER_SZ * MAXHEADERS))
    const u32 = new Uint32Array(u8.buffer)
    u32.ptr = spin.getAddress(u8.buffer)
    return u32
  }

  function handleChunked (response) {
    const { contentType, status, body, fd } = response
    spin.writeLatin1(sendbuf.id, `${responses[contentType][status].chunked}\r\n${body}`)
    send(fd, sendbuf.ptr, sendbuf.written, 0)
  }

  function handleNonChunked (response) {
    const { contentType, status, body, fd } = response
    spin.writeLatin1(sendbuf.id, `${responses[contentType][status].fixed}${body.length}\r\n\r\n${body}`)
    const written = send(fd, sendbuf.ptr, sendbuf.written, 0)
    if (written === sendbuf.written) return
    if (written > 0 || system.errno === EAGAIN) {
      console.log('write blocked')
      return
    }
    console.log(system.strerror())
    eventLoop.remove(fd)
    close(fd)
  }

  function handleResponse (response, fd) {
    response.fd = fd
    if (response.chunked) {
      handleChunked(response)
      return
    }
    handleNonChunked(response)
  }

  function onSocketEvent (fd) {
    const bytes = recv(fd, buf.ptr, BUFSIZE, 0)
    if (bytes > 0) {
      const nread = pico.parseRequest(buf.ptr, bytes, state.ptr)
      if (nread === bytes) {
        if (mode === modes.Sync) {
          if (nargs === 0) {
            handleResponse(fetch(), fd)
            return
          }
          if (nargs === 1) {
            const request = new Request(state, spin.readLatin1(buf.id))
            handleResponse(fetch(request), fd)
            return
          }
        }
      }
      console.log(`${nread}:${bytes}`)
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

  const { address = '127.0.0.1', port = 3000, serverName = 'p' } = config
  const HTTP_CTX_SZ = 32
  const HTTP_HEADER_SZ = 32
  const MAXHEADERS = 14
  const BUFSIZE = 4096
  const buf = new spin.RawBuffer(BUFSIZE)
  const state = httpState()
  const sendbuf = new spin.RawBuffer(BUFSIZE)
  const eventLoop = new Loop()
  const sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)
  spin.assert(sfd !== -1, 'socket', SystemError)
  spin.assert(setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, net.on, 32) === 0, 'setsockopt', SystemError)
  spin.assert(bind(sfd, sockaddr_in(address, port).address, SOCKADDR_LEN) === 0, 'bind', SystemError)
  spin.assert(listen(sfd, 1024) === 0, 'listen', SystemError)
  spin.assert(eventLoop.add(sfd, onConnect) === 0, 'eventLoop.add', SystemError)
  const mode = fetch.constructor.name === 'AsyncFunction' ? modes.Async: modes.Sync
  const nargs = fetch.length
  console.log(`listening on ${address}:${port} with ${mode} handler with ${nargs} args`)
  let time = (new Date()).toUTCString()
  const responses = {}
  const ckeys = Object.keys(contentTypes)
  const skeys = Object.keys(statusMessages)
  ckeys.forEach(k => responses[k] = {})
  function createResponses () {
    time = (new Date()).toUTCString()
    ckeys.forEach(contentType => {
      skeys.forEach(status => {
        responses[contentType][status] = {
          fixed: `HTTP/1.1 ${status} ${statusMessages[status]}\r\nContent-Type: ${contentTypes[contentType]}\r\nDate: ${time}\r\nContent-Length: `,
          chunked: `HTTP/1.1 ${status} ${statusMessages[status]}\r\nContent-Type: ${contentTypes[contentType]}\r\nDate: ${time}\r\nTransfer-Encoding: chunked\r\n`
        }
      })
    })
  }
  createResponses()
  while (1) {
    spin.runMicroTasks()
    if (eventLoop.poll(-1) <= 0) break
  }
}

export { serve, Response, Request }
