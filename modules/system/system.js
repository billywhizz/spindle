const api = {
  clock_gettime: {
    parameters: ['i32', 'pointer'],
    pointers: [, 'timespec*'],
    result: 'i32'
  },
  getpid: {
    parameters: [],
    result: 'i32'
  },
  getrusage: {
    parameters: ['i32', 'pointer'],
    pointers: [, 'struct rusage*'],
    result: 'i32'
  }
}

const includes = ['sys/resource.h', 'unistd.h']
const name = 'system'

export { api, includes, name }
