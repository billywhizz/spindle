// use ffi to build itself simplest possible runtime out of box with bindings for libffi
import { bindings, linkerScript, headerFile } from './tools/gen.js'

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
