#!/bin/bash

set -e
set -o pipefail

rm -rf test_crc || true
gcc test_crc.c bkv.c bkv_plus.c -o test_crc -g
./test_crc || true
rm -rf test_crc