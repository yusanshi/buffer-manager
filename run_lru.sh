#!/bin/bash
set -e

./build.sh
./manager.out -r LRU 2>&1 | tee lru.log
