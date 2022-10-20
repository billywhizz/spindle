#include "main.h"

int main(int argc, char** argv) {
  setvbuf(stdout, nullptr, _IONBF, 0);
  setvbuf(stderr, nullptr, _IONBF, 0);
  std::unique_ptr<Platform> platform = v8::platform::NewDefaultPlatform();
  V8::InitializePlatform(platform.get());
  V8::SetFlagsFromString(v8flags);
  if (_v8flags_from_commandline == 1) {
    V8::SetFlagsFromCommandLine(&argc, argv, true);
  }
  V8::Initialize();
  register_builtins();
  // todo. create isolate on a separate thread and wait for it
  // we can then spin up multiple isolates
  // need thread safe shared data
  spin::CreateIsolate(argc, argv, main_js, main_js_len, spin::hrtime(), GLOBALOBJ);
  //V8::Dispose();
  //platform.reset();
  return 0;
}
