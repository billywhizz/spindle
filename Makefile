CC=g++
RELEASE=0.1.13
INSTALL=/usr/local/bin
TARGET=spin
GLOBALOBJ="spin"
LIB=-ldl -lz
FLAGS=${CFLAGS}
LFLAG=${LFLAGS}
MODULE_DIR=modules
SPIN_HOME=$(shell pwd)
MODULES=modules/pico/binding.o modules/pico/pico.o modules/pico/hescape.o modules/pico/picohttpparser.o modules/net/net.o modules/loop/loop.o modules/system/system.o

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps/v8/libv8_monolith.a: ## download v8 monolithic library for linking
	curl -L -o v8lib-$(RELEASE).tar.gz https://raw.githubusercontent.com/just-js/libv8/$(RELEASE)/v8.tar.gz
	tar -zxvf v8lib-$(RELEASE).tar.gz
	rm -f v8lib-$(RELEASE).tar.gz

builtins.o: ## compile builtins with build dependencies
	gcc builtins.S -c -o builtins.o

compile:
	$(CC) -c ${FLAGS} -DGLOBALOBJ='${GLOBALOBJ}' -std=c++17 -DV8_COMPRESS_POINTERS -I. -I./deps/v8/include -g -O3 -march=native -mtune=native -Wpedantic -Wall -Wextra -flto -Wno-unused-parameter main.cc
	$(CC) -c ${FLAGS} -DGLOBALOBJ='${GLOBALOBJ}' -DVERSION='"${RELEASE}"' -std=c++17 -DV8_COMPRESS_POINTERS -DV8_TYPED_ARRAY_MAX_SIZE_IN_HEAP=0 -I. -I./deps/v8/include -g -O3 -march=native -mtune=native -Wpedantic -Wall -Wextra -flto -Wno-unused-parameter ${TARGET}.cc

main: deps/v8/libv8_monolith.a ## link the main application dynamically
	$(CC) -g -static-libstdc++ -flto -pthread -m64 -Wl,--start-group main.o deps/v8/libv8_monolith.a ${TARGET}.o builtins.o ${MODULES} -Wl,--end-group ${LFLAG} ${LIB} -o ${TARGET} -Wl,-rpath=/usr/local/lib/${TARGET}

main-static: deps/v8/libv8_monolith.a ## link the main application statically
	$(CC) -g -static -flto -pthread -m64 -Wl,--start-group deps/v8/libv8_monolith.a ${TARGET}.o builtins.o ${MODULES} main.o -Wl,--end-group ${LFLAG} ${LIB} -o ${TARGET} -Wl,-rpath=/usr/local/lib/${TARGET}

debug: ## strip debug symbols into a separate file
	objcopy --only-keep-debug ${TARGET} ${TARGET}.debug
	strip --strip-debug --strip-unneeded ${TARGET}
	objcopy --add-gnu-debuglink=${TARGET}.debug ${TARGET}

module: ## build a shared library for a module 
	CFLAGS="$(FLAGS)" LFLAGS="${LFLAG}" SPIN_HOME="$(SPIN_HOME)" $(MAKE) -C ${MODULE_DIR}/${MODULE}/ library

all:
	$(MAKE) clean builtins.o compile main debug

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
