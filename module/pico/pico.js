const api = {
  zlib_deflate: {
    parameters: ['pointer', 'u32', 'pointer', 'u32'],
    pointers: ['uint8_t*', ,'uint8_t*'],
    result: 'u32',
    name: 'zlibDeflate'
  },
  zlib_inflate: {
    parameters: ['pointer', 'u32', 'pointer', 'u32'],
    pointers: ['uint8_t*', ,'uint8_t*'],
    result: 'u32',
    name: 'zlibInflate'
  },
  parse_request: {
    parameters: ['pointer', 'u32', 'pointer'],
    pointers: ['char*', ,'httpRequest*'],
    result: 'i32',
    name: 'parseRequest'
  },
  parse_response: {
    parameters: ['pointer', 'u32', 'pointer'],
    pointers: ['char*', ,'httpResponse*'],
    result: 'i32',
    name: 'parseResponse'
  }
}

const includes = ['pico.h']
const name = 'pico'

export { api, includes, name }
