#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bkv.h"

void test_base() {
    char *string = "Hello, world";
    uint8_t array[3] = {1, 2, 3};
    uint8_t* p = array;

    bkv_dump_buf("def", p, 3);

    uint16_t value = 1440;
    uint8_t value_buf[2] = { (value & 0xFF00) >> 8, value & 0xFF };
}

void test_encode_decode() {
    char *string = "Hello, world";
    int size = 1024;
    uint8_t data[size];
    memset(data, 0, size);

    uint8_t key[1] = {2};
    uint8_t value[3] = {3, 4, 5};
    
    int offset = 0;
    // add kv: 2 -> Hello, world
    offset += bkv_append(data + offset, size - offset, key, 1, 0, (uint8_t* )string, strlen(string));
    // add kv: 2 -> 0x030405
    offset += bkv_append(data + offset, size - offset, key, 1, 0, value, 3);
    // add kv: num -> 6396
    offset += bkv_append_number_value_by_string_key(data + offset, size - offset, "num", 6396);
    // add kv: dd -> 0x303132
    uint8_t value_string[3] = {0x30, 0x31, 0x32};
    offset += bkv_append_by_string_key(data + offset, size - offset, "dd", value_string, 3);
    // add kv: 3 -> num(3)
    offset += bkv_append_number_value_by_number_key(data + offset, size - offset, 3, 3);
    // add kv: n3 -> num(3)
    offset += bkv_append_number_value_by_string_key(data + offset, size - offset, "n3", -1);
    // add kv: 6 -> '33'
    offset += bkv_append_string_value_by_number_key(data + offset, size - offset, 6, "33");
    // add kv: n6 -> '33'
    offset += bkv_append_string_value_by_string_key(data + offset, size - offset, "n6", "33");

    // robust check
    int append_offset = bkv_append_by_number_key(data + offset, size - offset, 99, value, 3);
    if (append_offset == -1) {
        LOGE("append fail");
        return;
    } else {
        offset += append_offset;
    }
    // bkv_dump_buf("encode", data, pos);

    int count = bkv_get_count(data, offset);
    LOGI("bkv count: %d", count);

    bkv_dump(data, offset);

    int contains_number_key = bkv_contains_number_key(data, offset, 2);
    LOGI("contains_number_key: %s", contains_number_key ? "true" : "false");
    int value_pos_begin = 0;
    int value_pos_end = 0;
    int result_code = bkv_get_value_by_number_key(data, offset, 2, &value_pos_begin, &value_pos_end);
    if (result_code == 0) {
        LOGI("bkv_get_value_by_number_key result: %d", result_code);
        bkv_dump_buf("value", data + value_pos_begin, value_pos_end - value_pos_begin);
    }

    int contains_string_key = bkv_contains_string_key(data, offset, "dd");
    LOGI("contains_string_key: %s", contains_string_key ? "true" : "false");
    result_code = bkv_get_value_by_string_key(data, offset, "dd", &value_pos_begin, &value_pos_end);
    if (result_code == 0) {
        LOGI("bkv_get_value_by_string_key result: %d", result_code);
        bkv_dump_buf("value", data + value_pos_begin, value_pos_end - value_pos_begin);
    }

    uint64_t get_num = 0;
    result_code = bkv_get_number_value_by_string_key(data, offset, "num", &get_num);
    if (result_code == 0) {
        LOGI("bkv_get_number_value_by_string_key result: %d", result_code);
        LOGI("num=%lld", (long long)get_num);
    }

    get_num = 0;
    result_code = bkv_get_number_value_by_string_key(data, offset, "n3", &get_num);
    if (result_code == 0) {
        LOGI("bkv_get_number_value_by_string_key result: %d", result_code);
        LOGI("num=%lld", get_num);
    }


    char n6[1024];
    result_code = bkv_get_string_value_by_string_key(data, offset, "n6", n6);
    if (result_code == 0) {
        LOGI("bkv_get_string_value_by_string_key result: %d", result_code);
        LOGI("value=%s, strlen=%d", n6, strlen(n6));
    }

    result_code = bkv_get_string_value_by_number_key(data, offset, 6, n6);
    if (result_code == 0) {
        LOGI("bkv_get_string_value_by_number_key result: %d", result_code);
        LOGI("value=%s, strlen=%d", n6, strlen(n6));
    }
}

int main() {
    // test_base();
    test_encode_decode();

    return 0;
}

