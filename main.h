#pragma once

#include "spin.h"

using v8::V8;
using v8::Platform;
extern char _binary_main_js_start[];
extern char _binary_main_js_end[];

extern "C" {
  extern void* _register_pico();
  extern void* _register_loop();
  extern void* _register_net();
  extern void* _register_system();
}

void register_builtins() {
  spin::builtins_add("main.js", _binary_main_js_start, _binary_main_js_end - _binary_main_js_start);
  spin::modules["pico"] = &_register_pico;
  spin::modules["net"] = &_register_net;
  spin::modules["loop"] = &_register_loop;
  spin::modules["system"] = &_register_system;
}
static unsigned int smol_js_len = _binary_main_js_end - _binary_main_js_start;
static const char* smol_js = _binary_main_js_start;
//static const char* v8flags = "--stack-trace-limit=10 --use-strict --turbo-fast-api-calls --allow-natives-syntax";
static const char* v8flags = "--stack-trace-limit=10 --use-strict --turbo-fast-api-calls";
static unsigned int _v8flags_from_commandline = 1;
