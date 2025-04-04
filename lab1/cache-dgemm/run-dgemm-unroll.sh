#!/bin/sh

/lib/linux-tools/5.15.0-134-generic/perf stat -e branch-misses taskset --cpu-list 0 ./dgemm-unroll

# taskset --cpu-list 0 valgrind --tool=callgrind --branch-sim=yes ./dgemm-unroll
