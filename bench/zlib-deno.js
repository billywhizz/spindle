const { ops, encode } = Deno.core
const buf = encode('0123456789'.repeat(1024))

function gzip2 (buf) {
  const rid = ops.op_compression_new('gzip', false)
  ops.op_compression_write(rid, buf)
  ops.op_compression_finish(rid)
}

import { gzip } from "https://deno.land/x/compress@v0.4.4/mod.ts"
import { dump } from '../scratch/pretty.js'

const rid = ops.op_compression_new('gzip', false)
const b1 = ops.op_compression_write(rid, buf)
console.log(dump(b1, b1.length, 0, 16, 0, false))
const b2 = ops.op_compression_finish(rid)
console.log(dump(b2, b2.length, 0, 16, 0, false))

const compressed = gzip(buf)
console.log(dump(compressed, compressed.length, 0, 16, 0, false))

function bench (count) {
  const start = Date.now()
  for (let i = 0; i < count; i++) gzip2(buf)
  const elapsed = Date.now() - start
  const rate = Math.floor(count / (elapsed / 1000))
  console.log(`time ${elapsed} ms rate ${rate}`)
}

const repeat = Number(Deno.args[0] || 10)
const count = Number(Deno.args[1] || 10000)

for (let i = 0; i < repeat; i++) bench(count)