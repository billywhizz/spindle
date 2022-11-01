C=gcc
CC=g++
MODULE=net

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

library: ## compile shared library and dependencies
	$(CC) -c -flto -g -O3 -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I. -I$(SPIN_HOME) -I$(SPIN_HOME)/deps/v8/include -march=native -mtune=native -Wall -Wextra -Wno-unused-parameter -o ${MODULE}.o ${MODULE}.cc
	ar crsT ${MODULE}.a ${MODULE}.o
	$(CC) -flto -g -O3 -static-libstdc++ -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so
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
