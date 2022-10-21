## prerequisites

- x86_64
- 5+ Linux Kernel
- SSE4
- make, gcc, g++, curl, targz

## build

```
make all
```

## benches

```
./spin bench/system.js
./spin bench/http-parser.js
./spin bench/zlib.js
```

## examples

```
./spin examples/httpd.js
./spin examples/timer.js
```
