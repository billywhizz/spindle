#pragma once

#include "spin.h"

using v8::V8;
using v8::Platform;

extern char _binary_main_js_start[];
extern char _binary_main_js_end[];
extern char _binary_lib_system_js_start[];
extern char _binary_lib_system_js_end[];
extern char _binary_lib_loop_js_start[];
extern char _binary_lib_loop_js_end[];
extern char _binary_lib_net_js_start[];
extern char _binary_lib_net_js_end[];
extern char _binary_lib_pico_js_start[];
extern char _binary_lib_pico_js_end[];
extern char _binary_lib_gen_js_start[];
extern char _binary_lib_gen_js_end[];

extern "C" {
  extern void* _register_system();
  extern void* _register_loop();
  extern void* _register_net();
  extern void* _register_pico();
}

void register_builtins() {
  spin::builtins_add("main.js", _binary_main_js_start, _binary_main_js_end - _binary_main_js_start);
  spin::builtins_add("lib/system.js", _binary_lib_system_js_start, _binary_lib_system_js_end - _binary_lib_system_js_start);
  spin::builtins_add("lib/loop.js", _binary_lib_loop_js_start, _binary_lib_loop_js_end - _binary_lib_loop_js_start);
  spin::builtins_add("lib/net.js", _binary_lib_net_js_start, _binary_lib_net_js_end - _binary_lib_net_js_start);
  spin::builtins_add("lib/pico.js", _binary_lib_pico_js_start, _binary_lib_pico_js_end - _binary_lib_pico_js_start);
  spin::builtins_add("lib/gen.js", _binary_lib_gen_js_start, _binary_lib_gen_js_end - _binary_lib_gen_js_start);
  spin::modules["system"] = &_register_system;
  spin::modules["loop"] = &_register_loop;
  spin::modules["net"] = &_register_net;
  spin::modules["pico"] = &_register_pico;
}

static unsigned int main_js_len = _binary_main_js_end - _binary_main_js_start;
static const char* main_js = _binary_main_js_start;
static const char* v8flags = "--stack-trace-limit=10 --use-strict --turbo-fast-api-calls";
static unsigned int _v8flags_from_commandline = 1;

