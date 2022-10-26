// todo - make this an array so we can have different versions of same function with different sets of args
// i.e. we could have multiple syscall entries with different signatures
// in fact, we could just use syscall for *all* syscalls and use name field to identify them
const api = {
  clock_gettime: {
    parameters: ['i32', 'pointer'],
    pointers: [, 'timespec*'],
    result: 'i32'
  },
  exit: {
    parameters: ['i32'],
    result: 'void'
  },
  usleep: {
    parameters: ['u32'],
    result: 'i32'
  },
  getpid: {
    parameters: [],
    result: 'u32'
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
  sleep: {
    parameters: ['u32'],
    result: 'u32'
  },
  timerfd_settime: {
    parameters: ['i32', 'i32', 'pointer', 'pointer'],
    pointers: [, , 'const struct itimerspec*', 'struct itimerspec*'],
    result: 'i32'
  },
  fork: {
    parameters: [],
    result: 'i32'
  },
  waitpid: {
    parameters: ['i32', 'pointer', 'i32'],
    pointers: [, 'int*'],
    result: 'i32'
  },
  execvp: {
    parameters: ['pointer', 'pointer'],
    pointers: ['char*', 'char* const*'],
    result: 'i32'
  },
  sysconf: {
    parameters: ['i32'],
    result: 'u32'
  },
  syscall: {
    parameters: ['i32', 'i32', 'u32'],
    result: 'i32',
    name: 'pidfd_open'
  },
  getrlimit: {
    parameters: ['i32', 'pointer'],
    pointers: [, 'struct rlimit*'],
    result: 'i32'
  },
  setrlimit: {
    parameters: ['i32', 'pointer'],
    pointers: [, 'const struct rlimit*'],
    result: 'i32'
  },
  strerror_r: {
    parameters: ['i32', 'pointer', 'u32'],
    pointers: [, 'char*'],
    result: 'i32'
  },
  times: {
    parameters: ['pointer'],
    pointers: ['struct tms*'],
    result: 'i32'
  },
  sysconf: {
    parameters: ['i32'],
    pointers: [],
    result: 'u32'
  },
  get_avphys_pages: {
    parameters: [],
    pointers: [],
    result: 'u32'
  },
  signal: {
    parameters: ['i32', 'pointer'],
    pointers: [, 'sighandler_t'],
    rpointer: 'sighandler_t',
    result: 'pointer'
  }
}
// allow setting constants using dlsym
// todo - all generating handlers/wrappers for callbacks - function pointers
const includes = ['sys/times.h', 'sys/resource.h', 'unistd.h', 'dlfcn.h', 'sys/timerfd.h', 'sys/wait.h', 'sys/sysinfo.h', 'signal.h']
const name = 'system'
const libs = ['dl']
const obj = ['system.a']

export { api, includes, name, libs, obj }
