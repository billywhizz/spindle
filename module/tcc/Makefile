C=gcc
CC=g++
MODULE=tcc
TCC_VERSION=0.9.27

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps/tcc-${TCC_VERSION}/libtcc.a: ## dependencies
	mkdir -p deps
	curl -L -o deps/tcc-${TCC_VERSION}.tar.bz2 http://download.savannah.gnu.org/releases/tinycc/tcc-${TCC_VERSION}.tar.bz2
	tar -jxvf deps/tcc-${TCC_VERSION}.tar.bz2	-C deps/
	cd deps/tcc-${TCC_VERSION} && CFLAGS='-mstackrealign -fPIC -flto -O3' ./configure && cd ../../
	make -C deps/tcc-${TCC_VERSION}/ libtcc.a

library: deps/tcc-${TCC_VERSION}/libtcc.a ## compile shared library and dependencies
	$(CC) -c -g -O3 -flto -fpermissive -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -Ideps/tcc-${TCC_VERSION}/ -I. -I$(SPIN_HOME) -I$(SPIN_HOME)/deps/v8/include -march=native -mtune=native -Wall -Wextra -Wno-unused-parameter -o ${MODULE}.o ${MODULE}.cc
	ar crsT ${MODULE}.a ${MODULE}.o deps/tcc-${TCC_VERSION}/libtcc.a
	$(CC) -g -O3 -flto -static-libstdc++ -shared -pthread -m64 -Wl,--start-group ${MODULE}.o deps/tcc-${TCC_VERSION}/libtcc.a -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so
	objcopy --only-keep-debug ${MODULE}.so ${MODULE}.so.debug
	strip --strip-debug --strip-unneeded ${MODULE}.so

clean: ## tidy up
	rm -f ${MODULE}.o
	rm -f ${MODULE}.so
	rm -f ${MODULE}.so.debug
	
all: ## make all
	make clean
	make library

.DEFAULT_GOAL := help
