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
    printf("\n\n[test encode decode]\n");

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
    // add kv: zero -> 0
    offset += bkv_append_number_value_by_string_key(data + offset, size - offset, "zero", 0);
    // add kv: 0 -> 0
    offset += bkv_append_number_value_by_number_key(data + offset, size - offset, 0, 0);

    // robust check
    int append_offset = bkv_append_by_number_key(data + offset, size - offset, 99, value, 3);
    if (append_offset == -1) {
        LOGE("append fail");
        exit(1);
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
        LOGI("[n]key=2 result: %d", result_code);
        bkv_dump_buf("value", data + value_pos_begin, value_pos_end - value_pos_begin);

        int content_size = value_pos_end - value_pos_begin + 1;
        char content[content_size];
        memset(content, 0, content_size);
        memcpy(content, data + value_pos_begin, value_pos_end - value_pos_begin);
        if (strcmp(string, content) != 0) {
            LOGE("[n]key=2 wrong string content: %s", content);
            exit(1);
        }
    } else {
        LOGE("[n]key=2 fail: result=%d", result_code);
        exit(1);
    }

    int contains_string_key = bkv_contains_string_key(data, offset, "dd");
    LOGI("contains_string_key: %s", contains_string_key ? "true" : "false");
    result_code = bkv_get_value_by_string_key(data, offset, "dd", &value_pos_begin, &value_pos_end);
    if (result_code == 0) {
        LOGI("[s]key=dd result: %d", result_code);
        bkv_dump_buf("value", data + value_pos_begin, value_pos_end - value_pos_begin);

        int content_size = value_pos_end - value_pos_begin + 1;
        char content[content_size];
        memset(content, 0, content_size);
        memcpy(content, data + value_pos_begin, value_pos_end - value_pos_begin);
        if (strcmp("012", content) != 0) {
            LOGE("[s]key=dd wrong string content: %s", content);
            exit(1);
        }
    } else {
        LOGE("[s]key=dd fail: result=%d", result_code);
        exit(1);
    }

    uint64_t get_num = 0;
    result_code = bkv_get_number_value_by_string_key(data, offset, "num", &get_num);
    if (result_code == 0) {
        LOGI("[s]key=num result: %d", result_code);
        if (get_num != 6396) {
            LOGE("[s]key=num wrong number content: %llu", get_num);
            exit(1);
        }
    } else {
        LOGE("[s]key=num fail: result=%d", result_code);
        exit(1);
    }

    get_num = 0;
    result_code = bkv_get_number_value_by_string_key(data, offset, "n3", &get_num);
    if (result_code == 0) {
        LOGI("[s]key=n3 result: %d", result_code);
        if (get_num != -1) {
            LOGE("[s]key=n3 wrong number content: %llu", get_num);
            exit(1);
        }
    } else {
        LOGE("[s]key=n3 fail: result=%d", result_code);
        exit(1);
    }


    char n6[1024];
    result_code = bkv_get_string_value_by_string_key(data, offset, "n6", n6);
    if (result_code == 0) {
        LOGI("[s]key=n6 result: %d", result_code);
        if (strcmp("33", n6) != 0) {
            LOGE("[s]key=n6 wrong string content: %s", n6);
            exit(1);
        }
    } else {
        LOGE("[s]key=n6 fail: result=%d", result_code);
        exit(1);
    }

    char c6[1024];
    result_code = bkv_get_string_value_by_number_key(data, offset, 6, c6);
    if (result_code == 0) {
        LOGI("[n]key=6 result: %d", result_code);
        if (strcmp("33", c6) != 0) {
            LOGE("[n]key=6 wrong string content: %s", c6);
            exit(1);
        }
    } else {
        LOGE("[n]key=6 fail: result=%d", result_code);
        exit(1);
    }

    uint64_t zero = 1;
    result_code = bkv_get_number_value_by_string_key(data, offset, "zero", &zero);
    if (result_code == 0) {
        LOGI("[s]key=zero result: %d", result_code);
        if (zero != 0) {
            LOGE("[s]key=zero wrong number content: %llu", zero);
            exit(1);
        }
    } else {
        LOGE("[s]key=zero fail: result=%d", result_code);
        exit(1);
    }

    zero = 1;
    result_code = bkv_get_number_value_by_number_key(data, offset, 0, &zero);
    if (result_code == 0) {
        LOGI("[n]key=0 result: %d", result_code);
        if (zero != 0) {
            LOGE("[n]key=0 wrong number content: %llu", zero);
            exit(1);
        }
    } else {
        LOGE("[n]key=0 fail: result=%d", result_code);
        exit(1);
    }
}

void test_get_array_list() {
    printf("\n\n[test get array list]\n");

    int size = 1024 * 100;
    uint8_t data[size];
    memset(data, 0, size);

    int offset = 0;

    int num = 100;

    // append
    for (int i = 0; i < num; i++) {
        offset += bkv_append_number_value_by_string_key(data + offset, size - offset, "point", i);
    }
    
    // check
    int count = bkv_get_count(data, offset);
    if (count != num) {
        LOGE("num not equal");
        exit(1);
    }

    int key_count = 0;

    for (int i = 0; i < count; i++) {
        int pos_begin = 0;
        int pos_end = 0;
        int result = bkv_get_kv_by_index(data, offset, i, &pos_begin, &pos_end);
        if (result != 0) {
            LOGE("get kv by index=%d fail, result=%d", i, result);
            exit(1);
        }

        int is_string_key = 0;
        char string_key[BKV_MAX_STRING_KEY_LEN + 1];
        uint64_t number_key = 0;
        result = bkv_get_key_from_kv(data + pos_begin, pos_end - pos_begin, &is_string_key, string_key, BKV_MAX_STRING_KEY_LEN, &number_key);
        if (result != 0) {
            LOGE("get key by index=%d fail, result=%d", i, result);
            exit(1);
        }
        if (is_string_key != 1) {
            LOGE("get key by index=%d fail, not string key", i);
            exit(1);
        }

        if (strcmp(string_key, "point") != 0) {
            continue;
        }

        key_count++;

        int value_pos_begin = 0;
        result = bkv_get_value_from_kv(data + pos_begin, pos_end - pos_begin, &value_pos_begin);
        if (result != 0) {
            LOGE("get value from kv by index=%d fail, result=%d", i, result);
            exit(1);
        }

        uint64_t n = bkv_decode_number(data + pos_begin + value_pos_begin, pos_end - pos_begin - value_pos_begin);
        if (n != i) {
            LOGE("value from kv by index=%d invalid, v=%ld", i, n);
            exit(1);
        }

    }

    if (key_count != num) {
        LOGE("key_count not equal");
        exit(1);
    }
}

void test_get_array_list_2() {
    printf("\n\n[test get array list 2]\n");

    int size = 1024 * 100;
    uint8_t data[size];
    memset(data, 0, size);

    int offset = 0;

    int num = 100;

    // append
    for (int i = 0; i < num; i++) {
        offset += bkv_append_number_value_by_string_key(data + offset, size - offset, "point", i);
    }

    // check
    int count = bkv_get_count(data, offset);
    if (count != num) {
        LOGE("num not equal");
        exit(1);
    }

    int key_count = 0;

    for (int i = 0; i < count; i++) {
        int is_string_key = 0;
        char string_key[BKV_MAX_STRING_KEY_LEN + 1];
        uint64_t number_key = 0;
        int value_pos_begin = 0;
        int value_pos_end = 0;
        int result = bkv_get_key_value_by_index(data, offset, i, &is_string_key, string_key, BKV_MAX_STRING_KEY_LEN, &number_key, &value_pos_begin, &value_pos_end);
        if (result != 0) {
            LOGE("get kv by index=%d fail, result=%d", i, result);
            exit(1);
        }

        if (is_string_key != 1) {
            LOGE("get key by index=%d fail, not string key", i);
            exit(1);
        }

        if (strcmp(string_key, "point") != 0) {
            continue;
        }

        key_count++;

        uint64_t n = bkv_decode_number(data + value_pos_begin, value_pos_end - value_pos_begin);
        if (n != i) {
            LOGE("value from kv by index=%d invalid, v=%ld", i, n);
            exit(1);
        }

    }

    if (key_count != num) {
        LOGE("key_count not equal");
        exit(1);
    }
}

uint8_t* hexs_to_bytes(const char* hex_string)
{
    size_t len = strlen(hex_string);
    if (len % 2 != 0) {
        return NULL;
    }
    size_t bytes_len = len / 2;
    uint8_t* bytes = (uint8_t*) malloc(bytes_len);
    for (size_t i = 0, j = 0; j < bytes_len; i += 2, j++) {
        bytes[j] = (hex_string[i] % 32 + 9) % 25 * 16 + (hex_string[i + 1] % 32 + 9) % 25;
    }
        
    return bytes;
}

float bytes_to_float(uint8_t *bytes, int big_endian) {
    float f;
    uint8_t *f_ptr = (uint8_t *) &f;
    if (big_endian) {
        f_ptr[3] = bytes[0];
        f_ptr[2] = bytes[1];
        f_ptr[1] = bytes[2];
        f_ptr[0] = bytes[3];
    } else {
        f_ptr[3] = bytes[3];
        f_ptr[2] = bytes[2];
        f_ptr[1] = bytes[1];
        f_ptr[0] = bytes[0];
    }
    return f;
}

void test_float() {
    printf("\n\n[test float]\n");

    char* hex = "0B8266313F8CCCCD3F99999A078266323FA66666";
    int data_size = strlen(hex);
    uint8_t* data = hexs_to_bytes(hex);

    int pos_begin = 0;
    int pos_end = 0;
    int result = bkv_get_value_by_index(data, data_size, 1, &pos_begin, &pos_end);
    if (result != 0) {
        LOGE("get kv index=1 fail, result=%d", result);
        exit(1);
    }

    float f1 = bytes_to_float(data + pos_begin, 1);
    printf("f1=%.6f", f1);
    if (f1 != 1.3f) {
        LOGE("wrong f1");
        exit(1);
    }
}

void test_float_2() {
    printf("\n\n[test float 2]\n");

    char* hex = "0B8266313F8CCCCD3F99999A078266323FA66666";
    int data_size = strlen(hex);
    uint8_t* data = hexs_to_bytes(hex);

    int pos_begin = 0;
    int pos_end = 0;
    int result = bkv_get_value_by_index(data, data_size, 1, &pos_begin, &pos_end);
    if (result != 0) {
        LOGE("get kv index=1 fail, result=%d", result);
        exit(1);
    }

    float f1 = bkv_decode_float(data + pos_begin);
    printf("f1=%.6f", f1);
    if (f1 != 1.3f) {
        LOGE("wrong f1");
        exit(1);
    }
}

void test_float_3() {
    printf("\n\n[test float 3]\n");

    int size = 1024;
    uint8_t data[size];
    memset(data, 0, size);

    int offset = 0;
    // add kv: f1 -> 0.1f
    offset += bkv_append_float_value_by_string_key(data + offset, size - offset, "f1", 0.1f);
    // add kv: 1 -> 0.2f
    offset += bkv_append_float_value_by_number_key(data + offset, size - offset, 1, 0.2f);

    bkv_dump(data, offset);

    float fs1 = 0;
    int result_code = bkv_get_float_value_by_string_key(data, offset, "f1", &fs1);
    if (result_code == 0) {
        LOGI("[s]key=f1 result: %d", result_code);
        if (fs1 != 0.1f) {
            LOGE("[s]key=f1 wrong float content: %.6f", fs1);
            exit(1);
        }
    } else {
        LOGE("[s]key=f1 fail: result=%d", result_code);
        exit(1);
    }

    float fn1 = 0;
    result_code = bkv_get_float_value_by_number_key(data, offset, 1, &fn1);
    if (result_code == 0) {
        LOGI("[n]key=1 result: %d", result_code);
        if (fn1 != 0.2f) {
            LOGE("[n]key=1 wrong float content: %.6f", fn1);
            exit(1);
        }
    } else {
        LOGE("[n]key=1 fail: result=%d", result_code);
        exit(1);
    }


}

int main() {
    // test_base();
    test_encode_decode();

    test_get_array_list();

    // more simple way
    test_get_array_list_2();

    test_float();

    test_float_2();

    test_float_3();

    return 0;
}

