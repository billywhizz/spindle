import { generate } from './tools/gen.js'

const source = await generate(spin.args[2])

console.log(source)