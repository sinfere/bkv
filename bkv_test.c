#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bkv.h"

void test_base() {
    char *string = "Hello, world";
    u_int8_t array[3] = {1, 2, 3};
    u_int8_t* p = array;
    
    dump_buf("def", p, 3);
}

void test_encode_decode() {
    char *string = "Hello, world";
    u_int8_t data[60];
    memset(data, 0, 60);

    u_int8_t key[1] = {2};
    u_int8_t value[3] = {3, 4, 5};
    int pos = bkv_append(data, 60, 0, key, 1, 0, value, 3);
    pos = bkv_append(data, 60, pos, key, 1, 0, value, 3);
    pos = bkv_append_by_string_key(data, 60, pos, "dd", value, 3);

    // robust check
    int pos_after_apend = bkv_append_by_number_key(data, 60, pos, 99, value, 3);
    if (pos_after_apend == -1) {
        LOGE("append fail");
        return;
    } else {
        pos = pos_after_apend;
    }

    int count = bkv_get_count(data, pos);
    LOGI("bkv count: %d", count);

    dump_bkv(data, pos);
    
    dump_buf("encode", data, pos);
}

int main() {
    // test_base();
    test_encode_decode();

    return 0;
}

