./bun version.js 
./spindle-full version.js 
./spindle-minimal version.js 
./spindle-minimal-static version.js 
apt install -y strace
strace -e openat ./bun version.js 
strace -e openat ./spindle-full version.js 
strace -e openat ./spindle-minimal version.js 
strace -e openat ./spindle-minimal-static version.js 
strace -c ./bun version.js 
strace -c ./spindle-full version.js 
strace -c ./spindle-minimal version.js 
strace -c ./spindle-minimal-static version.js 
taskset --cpu-list 0 ./hyperfine -r 1000 --warmup 300 "./bun version.js" "./spindle-full version.js" "./spindle-minimal version.js" "./spindle-minimal-static version.js"
