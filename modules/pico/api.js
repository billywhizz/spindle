const api = {
  zlib_deflate: {
    parameters: ['pointer', 'u32', 'pointer', 'u32'],
    result: 'u32'
  },
  zlib_inflate: {
    parameters: ['pointer', 'u32', 'pointer', 'u32'],
    result: 'u32'
  },
  parse_request: {
    parameters: ['pointer', 'u32', 'pointer'],
    result: 'i32'
  },
  parse_response: {
    parameters: ['pointer', 'u32', 'pointer'],
    result: 'i32'
  },
  hesc_escape_html: {
    parameters: ['pointer', 'pointer', 'u32'],
    result: 'u32'
  },
  hesc_escape_html16: {
    parameters: ['pointer', 'pointer', 'u32'],
    result: 'u32'
  },
}

export { api }
