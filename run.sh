#!/bin/bash
set -e

mkdir -p build
cd build
cmake ..
make
cd ..
mv build/manager manager.out
./manager.out 2>&1 | tee ${1:-manager}.log
