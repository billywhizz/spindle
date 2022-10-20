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

spin.assert = assert

global.console = {
  log: str => spin.print(str),
  error: str => spin.error(str)
}

global.onUnhandledRejection = err => {
  console.error(`${AG}${err.message}${AD}\n${err.stack}`)
  // todo: exit? maybe with a flag
}

async function main () {
  try {
    const { main, serve } = await import(spin.args[1])
    if (main) {
      await main(...spin.args.slice(2))
    }
    if (serve) {
      await serve(...spin.args.slice(2))
    }
  } catch (err) {
    //console.error(err.stack)
  }
}

if (spin.args.length > 1) {
  if (spin.args[1] === 'gen') {
    const { bindings, linkerScript, headerFile } = await import('lib/gen.js')
    let source = ''
    if (spin.args[2] === '--link') {
      source += await linkerScript('main.js')
      for (const fileName of spin.args.slice(3)) {
        source += await linkerScript(fileName)
      }
    } else if (spin.args[2] === '--header') {
      source = await headerFile(spin.args.slice(3))
    } else {
      source = await bindings(spin.args[2])
    }
    console.log(source)  
  } else {
    await main(...spin.args.slice(1))
  }
} else {
  console.log(JSON.stringify(spin.version))
}
