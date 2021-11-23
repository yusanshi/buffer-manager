#!/bin/bash
set -e

mkdir -p build
cd build
cmake ..
make
cd ..
mv build/manager manager.out

for replacement in LRU LRU_2 CLOCK
do
    for threads in 1 2 4
    do
        echo "Running with ${replacement} rereplacement policy, ${threads} thread(s)"
        ./manager.out -r ${replacement} -t ${threads} > ./log/replacement_${replacement}_threads_${threads}.log
    done
done
