import { ffi } from 'lib/ffi.js'
import { tcc } from 'lib/tcc.js'

const source = spin.CString(`
extern char** environ;

char* checkenv (int index) {
  return environ[index];
}
`)

const TCC_OUTPUT_MEMORY = 1
const TCC_RELOCATE_AUTO = 1
const ptr = tcc.tcc_new()
tcc.tcc_set_output_type(ptr, TCC_OUTPUT_MEMORY)
tcc.tcc_set_options(ptr, spin.CString('03').ptr)
tcc.tcc_set_options(ptr, spin.CString('nostdlib').ptr)
let rc = tcc.tcc_compile_string(ptr, source.ptr)
spin.assert(rc === 0, `could not compile (${rc})`)
rc = tcc.tcc_relocate(ptr, TCC_RELOCATE_AUTO)
spin.assert(rc === 0, `could not relocate (${rc})`)
const sym = spin.CString('checkenv')
const address = tcc.tcc_get_symbol(ptr, sym.ptr)
spin.assert(address, `could not locate symbol (${sym})`)
const env = ffi.wrap('checkenv', address, 'pointer', ['i32'])

let index = 0
let next = env(index++)
while (next) {
  console.log(spin.readLatin1Address(BigInt(next)))
  next = env(index++)
}
