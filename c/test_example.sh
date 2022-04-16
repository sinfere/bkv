#!/bin/bash

set -e
set -o pipefail

rm -rf test || true
gcc bkv_example.c bkv.c bkv_plus.c -o test_example -g
./test_example || true
rm -rf test