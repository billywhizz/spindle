C=gcc
CC=g++
MODULE=pico

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

library: ## compile shared library and dependencies
	$(C) -c -fPIC -g -O3 -Wall -Wextra -march=native -mtune=native -m64 -msse4.2 -std=c11 -o hescape.o hescape.c
	$(C) -c -fPIC -g -O3 -Wall -Wextra -march=native -mtune=native -m64 -msse4.2 -std=c11 -o picohttpparser.o picohttpparser.c
	$(C) -c -fPIC -g -O3 -Wall -Wextra -march=native -mtune=native -m64 ${MODULE}.c
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I. -I$(SPIN_HOME) -I$(SPIN_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter -o binding.o ${MODULE}.cc
#	$(C) -g -nostdlib -shared -flto -pthread -m64 -Wl,--start-group picohttpparser.o hescape.o ${MODULE}.o -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so
	$(CC) -g -static-libstdc++ -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o picohttpparser.o hescape.o binding.o -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so
	objcopy --only-keep-debug ${MODULE}.so ${MODULE}.so.debug
	strip --strip-debug --strip-unneeded ${MODULE}.so

clean: ## tidy up
	rm -f hescape.o picohttpparser.o ${MODULE}.o binding.o
	rm -f ${MODULE}.so
	rm -f ${MODULE}.so.debug
	
all: ## make all
	make clean
	make library

.DEFAULT_GOAL := help
