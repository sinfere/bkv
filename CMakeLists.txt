cmake_minimum_required(VERSION 3.4.1)
project(BKV)
set(CMAKE_CXX_STANDARD 14)

set(SOURCES bkv.c)

add_library(bkv SHARED ${SOURCES})

add_executable(bkv_test bkv_test.c)



target_link_libraries(bkv_test bkv)

add_executable(bkv_test_plus bkv.c bkv_test_plus.cpp )

target_link_libraries(bkv_test_plus bkv)