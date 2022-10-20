const { pico } = spin.load('pico')

const repeat = Number(spin.args[2] || 1)
const count = Number(spin.args[3] || 30000000)

function httpState () {
  const u8 = new Uint8Array(HTTP_CTX_SZ + (HTTP_HEADER_SZ * MAXHEADERS))
  const u32 = new Uint32Array(u8.buffer)
  u32.ptr = spin.getAddress(u8.buffer)
  return u32
}

function benchResponse () {
  const start = Date.now()
  for (let i = 0; i < count; i++) parseResponse()
  const elapsed = Date.now() - start
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${elapsed} ms rate ${rate}`)
}

function benchRequest () {
  const start = Date.now()
  for (let i = 0; i < count; i++) parseRequest()
  const elapsed = Date.now() - start
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${elapsed} ms rate ${rate}`)
}

const HTTP_CTX_SZ = 32
const HTTP_HEADER_SZ = 32
const MAXHEADERS = 14

const responseBuf = spin.calloc(1, 'HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n')
const response = spin.getAddress(responseBuf)
const responseSize = responseBuf.byteLength

const requestBuf = spin.calloc(1, 'GET / HTTP/1.1\r\nHost: foo.bar.baz1\r\n\r\n')
const request = spin.getAddress(requestBuf)
const requestSize = requestBuf.byteLength

const state = httpState()
const info = state.ptr

function parseRequest () {
  return pico.parseRequest(request, requestSize, info)
}

function parseResponse () {
  return pico.parseResponse(response, responseSize, info)
}

spin.assert(parseRequest() === parseResponse() && parseRequest() === 38)

while (1) {
  for (let i = 0; i < repeat; i++) benchResponse()
  for (let i = 0; i < repeat; i++) benchRequest()
}
