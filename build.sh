#!/bin/bash
export SPIN_HOME=$(pwd)
./spin tools/idl.js ./modules/pico/pico.js > ./modules/pico/pico.cc
./spin tools/idl.js ./modules/loop/loop.js > ./modules/loop/loop.cc
./spin tools/idl.js ./modules/net/net.js > ./modules/net/net.cc
./spin tools/idl.js ./modules/system/system.js > ./modules/system/system.cc
MODULE=net make module
MODULE=loop make module
MODULE=system make module
MODULE=pico make module
rm -f builtins.o
make clean builtins.o compile main debug
