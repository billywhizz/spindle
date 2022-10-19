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
  },
  dlopen: {
    parameters: ['pointer', 'i32'],
    pointers: ['const char*'],
    result: 'pointer'
  },
  dlsym: {
    parameters: ['pointer', 'pointer'],
    pointers: ['void*', 'const char*'],
    result: 'pointer'
  },
  dlclose: {
    parameters: ['pointer'],
    pointers: ['void*'],
    result: 'i32'
  }
}

const includes = ['sys/resource.h', 'unistd.h', 'dlfcn.h']
const name = 'system'

export { api, includes, name }
