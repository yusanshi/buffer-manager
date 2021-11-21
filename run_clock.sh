#!/bin/bash
set -e

./build.sh
./manager.out -r CLOCK 2>&1 | tee clock.log
