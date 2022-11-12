const api = {
  hesc_escape_html: {
    parameters: ['pointer', 'pointer', 'u32'],
    pointers: ['uint8_t*', 'const uint8_t*'],
    result: 'u32',
    name: 'escapeHTML'
  },
  hesc_escape_html16: {
    parameters: ['pointer', 'pointer', 'u32'],
    pointers: ['uint8_t*', 'const uint8_t*'],
    result: 'u32',
    name: 'escapeHTML16'
  }
}

const includes = ['hescape.h']
const name = 'html'

export { api, includes, name }
