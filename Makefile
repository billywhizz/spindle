CC=g++
RELEASE=0.1.14
INSTALL=/usr/local/bin
TARGET=spin
GLOBALOBJ="spin"
LIB=-ldl
FLAGS=${CFLAGS}
LFLAG=${LFLAGS}
MODULE_DIR=module
SPIN_HOME=$(shell pwd)
MODULES=${MODULE_DIR}/system/system.a ${MODULE_DIR}/loop/loop.a ${MODULE_DIR}/net/net.a ${MODULE_DIR}/pico/pico.a ${MODULE_DIR}/fs/fs.a ${MODULE_DIR}/html/html.a
LIBS=lib/system.js lib/loop.js lib/net.js lib/pico.js lib/gen.js
DEPS=deps/v8/libv8_monolith.a deps/zlib-ng-2.0.6/libz.a
NPROCS=$(shell cat /proc/cpuinfo | grep processor | wc -l)

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps/zlib-ng-2.0.6/libz.a: ## build zlib-ng
	mkdir -p deps
	curl -L -o zlib-ng.tar.gz https://github.com/zlib-ng/zlib-ng/archive/refs/tags/2.0.6.tar.gz
	tar -zxvf zlib-ng.tar.gz -C deps/
	rm -f zlib-ng.tar.gz
	cd deps/zlib-ng-2.0.6 && ./configure --zlib-compat && make -j ${NPROCS}

deps/v8/libv8_monolith.a: ## download v8 monolithic library for linking
	mkdir -p deps
	curl -L -o v8lib-$(RELEASE).tar.gz https://raw.githubusercontent.com/just-js/libv8/$(RELEASE)/v8.tar.gz
	tar -zxvf v8lib-$(RELEASE).tar.gz
	rm -f v8lib-$(RELEASE).tar.gz

builtins.o: ## compile builtins with build dependencies
	gcc -flto builtins.S -c -o builtins.o

gen: ## generate source from definitions
	./spin gen --link ${LIBS} > builtins.S
	./spin gen --header ${LIBS} ${MODULES} > main.h
	./spin gen ${MODULE_DIR}/system/system.js > ${MODULE_DIR}/system/system.cc
	./spin gen ${MODULE_DIR}/pico/pico.js > ${MODULE_DIR}/pico/pico.cc
	./spin gen ${MODULE_DIR}/net/net.js > ${MODULE_DIR}/net/net.cc
	./spin gen ${MODULE_DIR}/loop/loop.js > ${MODULE_DIR}/loop/loop.cc
	./spin gen ${MODULE_DIR}/fs/fs.js > ${MODULE_DIR}/fs/fs.cc
	./spin gen ${MODULE_DIR}/tcc/tcc.js > ${MODULE_DIR}/tcc/tcc.cc

gen-min: ## generate source for minimal build
	./spin gen --link > builtins.S
	./spin gen --header > main.h

compile: ## compile the runtime
	$(CC) -flto -g -O3 -c ${FLAGS} -DGLOBALOBJ='${GLOBALOBJ}' -std=c++17 -DV8_COMPRESS_POINTERS -I. -I./deps/v8/include -march=native -mtune=native -Wpedantic -Wall -Wextra -Wno-unused-parameter main.cc
	$(CC) -flto -g -O3 -c ${FLAGS} -DGLOBALOBJ='${GLOBALOBJ}' -DVERSION='"${RELEASE}"' -std=c++17 -DV8_COMPRESS_POINTERS -DV8_TYPED_ARRAY_MAX_SIZE_IN_HEAP=0 -I. -I./deps/v8/include -march=native -mtune=native -Wpedantic -Wall -Wextra -Wno-unused-parameter ${TARGET}.cc

main: ${DEPS} ## link the runtime dynamically
	$(CC) -flto -g -O3 -rdynamic -fvisibility=hidden -pthread -m64 -Wl,--start-group main.o deps/v8/libv8_monolith.a ${TARGET}.o builtins.o ${DEPS} ${MODULES} -Wl,--end-group ${LFLAG} ${LIB} -o ${TARGET}

main-static: ${DEPS} ## link the runtime statically
	$(CC) -flto -g -O3 -static -fvisibility=hidden -pthread -m64 -Wl,--start-group main.o deps/v8/libv8_monolith.a ${TARGET}.o builtins.o ${DEPS} ${MODULES} -Wl,--end-group ${LFLAG} ${LIB} -o ${TARGET}

main-static-libc++: ${DEPS} ## link the runtime statically
	$(CC) -flto -g -O3 -rdynamic -fvisibility=hidden -static-libstdc++ -static-libgcc -pthread -m64 -Wl,--start-group main.o deps/v8/libv8_monolith.a ${TARGET}.o builtins.o ${DEPS} ${MODULES} -Wl,--end-group ${LFLAG} ${LIB} -o ${TARGET}

debug: ## strip debug symbols into a separate file
	objcopy --only-keep-debug ${TARGET} ${TARGET}.debug
	strip --strip-debug --strip-unneeded ${TARGET}
	objcopy --add-gnu-debuglink=${TARGET}.debug ${TARGET}

gen-library: ## generate source from definitions for a library
	./spin gen ${MODULE_DIR}/${MODULE}/${MODULE}.js > ${MODULE_DIR}/${MODULE}/${MODULE}.cc

library: ## build a module
	CFLAGS="$(FLAGS)" LFLAGS="${LFLAG}" SPIN_HOME="$(SPIN_HOME)" $(MAKE) -C ${MODULE_DIR}/${MODULE}/ library

scc: ## report on code size
	scc --no-cocomo --exclude-dir="deps,bench,test,.devcontainer,.git,.vscode,scratch,example,doc,docker" --include-ext="cc,c,h,js" --gen --wide --by-file ./ > scc.txt

minimal: ## minimal build with no modules or libs
ifneq (,$(wildcard ./spin))
	$(MAKE) gen-min
endif
	$(MAKE) deps/v8/libv8_monolith.a
	rm -f builtins.o
	$(MAKE) LIBS= MODULES= builtins.o compile main-static-libc++ debug

minimal-static: ## minimal build with no modules or libs
ifneq (,$(wildcard ./spin))
	$(MAKE) gen-min
endif
	$(MAKE) deps/v8/libv8_monolith.a
	rm -f builtins.o
	$(MAKE) LIBS= MODULES= builtins.o compile main-static debug

full: ## build with all libs and modules included
ifneq (,$(wildcard ./spin))
	$(MAKE) gen clean
endif
	$(MAKE) deps/zlib-ng-2.0.6/libz.a
	$(MAKE) deps/v8/libv8_monolith.a
	$(MAKE) MODULE=net library
	$(MAKE) MODULE=system library
	$(MAKE) MODULE=loop library
	$(MAKE) MODULE=pico library
	$(MAKE) MODULE=fs library
	$(MAKE) MODULE=html library
	rm -f builtins.o
	$(MAKE) builtins.o compile main-static-libc++ debug

docker-distroless: ## build a distroless docker image
	$(MAKE) main-static debug
	cp ./spin docker/
	docker build -t spin -f docker/distroless.dockerfile ./docker

docker-builder: ## build a docker builder image for building spin from source
	docker build -t spin-builder -f docker/builder.dockerfile ./docker

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
