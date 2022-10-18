// use ffi to build itself simplest possible runtime out of box with bindings for libffi
import { generate } from './tools/gen.js'

const source = await generate(spin.args[2])

console.log(source)