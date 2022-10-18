const api = {
  clock_gettime: {
    parameters: ['i32', 'pointer'],
    pointers: [, 'timespec*'],
    result: 'i32'
  },
}

const includes = []
const name = 'system'

export { api, includes, name }
