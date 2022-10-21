CC=g++
RELEASE=0.1.13
INSTALL=/usr/local/bin
TARGET=spin
GLOBALOBJ="spin"
LIB=-ldl
FLAGS=${CFLAGS}
LFLAG=${LFLAGS}
MODULE_DIR=modules
SPIN_HOME=$(shell pwd)
MODULES=modules/system/system.a modules/loop/loop.a modules/net/net.a modules/pico/pico.a
LIBS=lib/system.js lib/loop.js lib/net.js lib/pico.js lib/gen.js

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps/zlib-ng-2.0.6/libz.a: ## build zlib-ng
	mkdir -p deps
	curl -L -o zlib-ng.tar.gz https://github.com/zlib-ng/zlib-ng/archive/refs/tags/2.0.6.tar.gz
	tar -zxvf zlib-ng.tar.gz -C deps/
	rm -f zlib-ng.tar.gz
	cd deps/zlib-ng-2.0.6 && ./configure --zlib-compat && make -j 8

deps/v8/libv8_monolith.a: ## download v8 monolithic library for linking
	mkdir -p deps
	curl -L -o v8lib-$(RELEASE).tar.gz https://raw.githubusercontent.com/just-js/libv8/$(RELEASE)/v8.tar.gz
	tar -zxvf v8lib-$(RELEASE).tar.gz
	rm -f v8lib-$(RELEASE).tar.gz

builtins.o: ## compile builtins with build dependencies
	gcc builtins.S -c -o builtins.o

gen: ## generate source from definitions
	./spin gen --link ${LIBS} > builtins.S
	./spin gen --header ${LIBS} ${MODULES} > main.h
	./spin gen modules/system/system.js > modules/system/system.cc
	./spin gen modules/pico/pico.js > modules/pico/pico.cc
	./spin gen modules/net/net.js > modules/net/net.cc
	./spin gen modules/loop/loop.js > modules/loop/loop.cc

compile: ## compile the runtime
	$(CC) -c ${FLAGS} -DGLOBALOBJ='${GLOBALOBJ}' -std=c++17 -DV8_COMPRESS_POINTERS -I. -I./deps/v8/include -g -O3 -march=native -mtune=native -Wpedantic -Wall -Wextra -flto -Wno-unused-parameter main.cc
	$(CC) -c ${FLAGS} -DGLOBALOBJ='${GLOBALOBJ}' -DVERSION='"${RELEASE}"' -std=c++17 -DV8_COMPRESS_POINTERS -DV8_TYPED_ARRAY_MAX_SIZE_IN_HEAP=0 -I. -I./deps/v8/include -g -O3 -march=native -mtune=native -Wpedantic -Wall -Wextra -flto -Wno-unused-parameter ${TARGET}.cc

main: deps/v8/libv8_monolith.a deps/zlib-ng-2.0.6/libz.a ## link the runtime dynamically
	$(CC) -g -static-libstdc++ -flto -pthread -m64 -Wl,--start-group main.o deps/v8/libv8_monolith.a ${TARGET}.o builtins.o deps/zlib-ng-2.0.6/libz.a ${MODULES} -Wl,--end-group ${LFLAG} ${LIB} -o ${TARGET} -Wl,-rpath=/usr/local/lib/${TARGET}

main-static: deps/v8/libv8_monolith.a deps/zlib-ng-2.0.6/libz.a ## link the runtime statically
	$(CC) -g -static -flto -pthread -m64 -Wl,--start-group deps/v8/libv8_monolith.a ${TARGET}.o builtins.o deps/zlib-ng-2.0.6/libz.a ${MODULES} main.o -Wl,--end-group ${LFLAG} ${LIB} -o ${TARGET} -Wl,-rpath=/usr/local/lib/${TARGET}

debug: ## strip debug symbols into a separate file
	objcopy --only-keep-debug ${TARGET} ${TARGET}.debug
	strip --strip-debug --strip-unneeded ${TARGET}
	objcopy --add-gnu-debuglink=${TARGET}.debug ${TARGET}

module: ## build a module
	CFLAGS="$(FLAGS)" LFLAGS="${LFLAG}" SPIN_HOME="$(SPIN_HOME)" $(MAKE) -C ${MODULE_DIR}/${MODULE}/ library

scc: ## report on code size
	scc --exclude-dir="deps,bench,tools,config,.devcontainer,.git,.vscode" --include-ext="cc,c,h,js" --wide --by-file ./ > scc.txt

all: ## build all the things
	$(MAKE) deps/zlib-ng-2.0.6/libz.a
	$(MAKE) deps/v8/libv8_monolith.a
	$(MAKE) MODULE=net module
	$(MAKE) MODULE=system module
	$(MAKE) MODULE=loop module
	$(MAKE) MODULE=pico module
	rm builtins.o
	$(MAKE) builtins.o compile main debug

clean: ## tidy up
	rm -f *.o
	rm -f *.gz
	rm -f spin
	rm -f *.debug

cleanall: ## remove target and build deps
	rm -fr deps
	$(MAKE) clean
	rm -f ${TARGET}
	rm -f ${TARGET}.debug

install: ## install
	mkdir -p ${INSTALL}
	cp -f ${TARGET} ${INSTALL}/${TARGET}

install-debug: ## install debug symbols
	mkdir -p ${INSTALL}/.debug
	cp -f ${TARGET}.debug ${INSTALL}/.debug/${TARGET}.debug

uninstall: ## uninstall
	rm -f ${INSTALL}/${TARGET}
	rm -f ${INSTALL}/${TARGET}/.debug

.DEFAULT_GOAL := help
