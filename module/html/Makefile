C=gcc
CC=g++
MODULE=html

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

library: ## compile shared library and dependencies
	$(C) -c -fPIC -flto -g -O3 -Wall -Wextra -march=native -mtune=native -m64 -msse4.2 -std=c11 -o hescape.o hescape.c
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I. -I$(SPIN_HOME) -I$(SPIN_HOME)/deps/v8/include -g -O3 -msse4.2 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter -o binding.o ${MODULE}.cc
	ar crsT ${MODULE}.a hescape.o binding.o
	$(CC) -g -static-libstdc++ -msse4.2 -shared -flto -O3 -pthread -m64 -Wl,--start-group ${MODULE}.a -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so
	objcopy --only-keep-debug ${MODULE}.so ${MODULE}.so.debug
	strip --strip-debug --strip-unneeded ${MODULE}.so

clean: ## tidy up
	rm -f hescape.o binding.o
	rm -f ${MODULE}.so
	rm -f ${MODULE}.so.debug
	
all: ## make all
	make clean
	make library

.DEFAULT_GOAL := help
