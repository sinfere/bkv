#!/bin/bash

set -e
set -o pipefail

rm -rf test || true
gcc bkv_test.c bkv.c -o test -g
./test || true
rm -rf test