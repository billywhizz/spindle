#!/bin/bash
make MODULE=test gen module main
echo "name,size,callbacks,ops,poll,rss,usr,sys,tot"
SIZES="1 2 4 8 16 32 64 128 256 512 1024 2048 4096"
for sz in $SIZES
do
  taskset --cpu-list 0 ./spin compare.js $sz 1
  taskset --cpu-list 0 ./spin compare.js $sz 2
  taskset --cpu-list 0 ./spin compare.js $sz 3
  taskset --cpu-list 0 ./spin compare.js $sz 4
done
