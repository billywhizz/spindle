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
  },
  timerfd_create: {
    parameters: ['i32', 'i32'],
    result: 'i32'
  },
  timerfd_settime: {
    parameters: ['i32', 'i32', 'pointer', 'pointer'],
    pointers: [, , 'const struct itimerspec*', 'struct itimerspec*'],
    result: 'i32'
  }
}

const includes = ['sys/resource.h', 'unistd.h', 'dlfcn.h', 'sys/timerfd.h']
const name = 'system'

export { api, includes, name }
