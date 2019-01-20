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

    u_int8_t value_string[3] = {0x30, 0x31, 0x32};
    pos = bkv_append_by_string_key(data, 60, pos, "dd", value_string, 3);

    // robust check
    int pos_after_apend = bkv_append_by_number_key(data, 60, pos, 99, value, 3);
    if (pos_after_apend == -1) {
        LOGE("append fail");
        return;
    } else {
        pos = pos_after_apend;
    }
    // dump_buf("encode", data, pos);

    int count = bkv_get_count(data, pos);
    LOGI("bkv count: %d", count);

    dump_bkv(data, pos);

    int contains_number_key = bkv_contains_number_key(data, pos, 2);
    LOGI("contains_number_key: %d", contains_number_key);
    int value_pos_begin = 0;
    int value_pos_end = 0;
    int result_get_value_by_number_key = bkv_get_value_by_number_key(data, pos, 2, &value_pos_begin, &value_pos_end);
    if (result_get_value_by_number_key == 0) {
        LOGI("result_get_value_by_number_key: %d", result_get_value_by_number_key);
        dump_buf("value", data + value_pos_begin, value_pos_end - value_pos_begin);
    }

    int contains_string_key = bkv_contains_string_key(data, pos, "dd");
    LOGI("contains_string_key: %d", contains_string_key);
    int result_get_value_by_string_key = bkv_get_value_by_string_key(data, pos, "dd", &value_pos_begin, &value_pos_end);
    if (result_get_value_by_string_key == 0) {
        LOGI("result_get_value_by_string_key: %d", result_get_value_by_string_key);
        dump_buf("value", data + value_pos_begin, value_pos_end - value_pos_begin);
    }    
    
    
}

int main() {
    // test_base();
    test_encode_decode();

    return 0;
}

