#!/bin/sh

# /lib/linux-tools/5.15.0-134-generic/perf  stat -e branch-misses taskset --cpu-list 0 ./prog

taskset --cpu-list 0 valgrind --tool=callgrind --branch-sim=yes ./prog


