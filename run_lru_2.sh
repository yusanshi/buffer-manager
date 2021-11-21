#!/bin/bash
set -e

./build.sh
./manager.out -r LRU_2 2>&1 | tee lru_2.log
