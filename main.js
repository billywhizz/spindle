const AD = '\u001b[0m' // ANSI Default
const A0 = '\u001b[30m' // ANSI Black
const AR = '\u001b[31m' // ANSI Red
const AG = '\u001b[32m' // ANSI Green
const AY = '\u001b[33m' // ANSI Yellow
const AB = '\u001b[34m' // ANSI Blue
const AM = '\u001b[35m' // ANSI Magenta
const AC = '\u001b[36m' // ANSI Cyan
const AW = '\u001b[37m' // ANSI White

spin.colors = { AD, AB, A0, AR, AG, AY, AM, AC, AW }

function assert (condition, message) {
  if (!condition) {
    throw new Error(message || "Assertion failed")
  }
}

class TextEncoder {
  encode (str) {
    return new Uint8Array(spin.calloc(1, str))
  }

  encodeInto (str, buf) {
    
  }
}

class TextDecoder {
  decode (u8) {
    return spin.readString(u8.buffer, u8.byteLength, u8.byteOffset)
  }
}

global.TextEncoder = TextEncoder
global.TextDecoder = TextDecoder

const encoder = new TextEncoder()
const decoder = new TextDecoder()

spin.encode = str => encoder.encode(str)
spin.decode = bytes => decoder.decode(bytes)

spin.assert = assert

global.console = {
  log: str => spin.print(str),
  error: str => spin.error(str)
}

global.onUnhandledRejection = err => {
  spin.error(err.message)
  spin.error(err.stack)
}

try {
  const { main, serve } = await import(spin.args[1] || 'app.js')
  if (main) {
    await main(...spin.args.slice(2))
  }
  if (serve) {
    await serve(...spin.args.slice(2))
  }
} catch (err) {
  //console.error(err.stack)
}
