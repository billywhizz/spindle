#pragma once

#include "spin.h"

using v8::V8;
using v8::Platform;
extern char _binary_main_js_start[];
extern char _binary_main_js_end[];
void register_builtins() {
  NAMESPACE::builtins_add("main.js", _binary_main_js_start, _binary_main_js_end - _binary_main_js_start);
}
static unsigned int smol_js_len = _binary_main_js_end - _binary_main_js_start;
static const char* smol_js = _binary_main_js_start;
static const char* v8flags = "--stack-trace-limit=10 --use-strict --turbo-fast-api-calls --allow-natives-syntax";
static unsigned int _v8flags_from_commandline = 1;
