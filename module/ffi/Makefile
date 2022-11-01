C=gcc
CC=g++
MODULE=ffi
FFI_VERSION=3.4.2
NPROCS=$(shell cat /proc/cpuinfo | grep processor | wc -l)
DEPS=deps/libffi-${FFI_VERSION}/x86_64-pc-linux-gnu/.libs/libffi.a
INCLUDES=-I./deps/libffi-${FFI_VERSION}/x86_64-pc-linux-gnu/include/

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps/libffi-${FFI_VERSION}/x86_64-pc-linux-gnu/.libs/libffi.a: ## download libffi
	mkdir -p deps
	curl -L -o libffi.tar.gz https://github.com/libffi/libffi/archive/refs/tags/v${FFI_VERSION}.tar.gz
	tar -zxvf libffi.tar.gz -C deps/
	rm -f libffi.tar.gz
	cd deps/libffi-${FFI_VERSION} && ./autogen.sh && CFLAGS='-mstackrealign -fPIC -flto -O3' ./configure --enable-static=yes --enable-shared=no --disable-docs && make -j ${NPROCS}

library: deps/libffi-${FFI_VERSION}/x86_64-pc-linux-gnu/.libs/libffi.a ## compile shared library and dependencies
	$(CC) -c -flto -g -O3 -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I. -I$(SPIN_HOME) ${INCLUDES} -I$(SPIN_HOME)/deps/v8/include -march=native -mtune=native -Wall -Wextra -Wno-unused-parameter -o ${MODULE}.o ${MODULE}.cc
	ar crsT ${MODULE}.a ${MODULE}.o
	$(CC) -flto -g -O3 -static-libstdc++ -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o ${DEPS} -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so
	objcopy --only-keep-debug ${MODULE}.so ${MODULE}.so.debug
	strip --strip-debug --strip-unneeded ${MODULE}.so

clean: ## tidy up
	rm -f ${MODULE}.o
	rm -f ${MODULE}.a
	rm -f ${MODULE}.so
	rm -f ${MODULE}.so.debug
	
all: ## make all
	make clean
	make library

.DEFAULT_GOAL := help
