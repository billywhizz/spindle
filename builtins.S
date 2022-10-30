.global _binary_main_js_start
_binary_main_js_start:
        .incbin "main.js"
        .global _binary_main_js_end
_binary_main_js_end:
.global _binary_lib_system_js_start
_binary_lib_system_js_start:
        .incbin "lib/system.js"
        .global _binary_lib_system_js_end
_binary_lib_system_js_end:
.global _binary_lib_loop_js_start
_binary_lib_loop_js_start:
        .incbin "lib/loop.js"
        .global _binary_lib_loop_js_end
_binary_lib_loop_js_end:
.global _binary_lib_net_js_start
_binary_lib_net_js_start:
        .incbin "lib/net.js"
        .global _binary_lib_net_js_end
_binary_lib_net_js_end:
.global _binary_lib_pico_js_start
_binary_lib_pico_js_start:
        .incbin "lib/pico.js"
        .global _binary_lib_pico_js_end
_binary_lib_pico_js_end:
.global _binary_lib_gen_js_start
_binary_lib_gen_js_start:
        .incbin "lib/gen.js"
        .global _binary_lib_gen_js_end
_binary_lib_gen_js_end:

