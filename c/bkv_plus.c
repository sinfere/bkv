//
// Created by dd on 2022/3/25.
//

#include "bkv_plus.h"

void bkv_dump_buf(char* name, const uint8_t* buf, int buf_size) {
    int i, j;

    printf("%-5s[%d]: ", name, buf_size);
    for (i = 0; i < buf_size; i++) {
        printf("%02X", *(buf + i));
    }

    // check printable string
    if (buf_size != 0) {
        uint8_t first_byte = *(buf);
        if (0x20 <= first_byte && first_byte <= 0x7E) {
            printf(" (s: ");
            for (j = 0; j < buf_size; j++) {
                printf("%c", *(buf + j));
            }
            printf(")");
        }
    }

    printf("\n");
}

void bkv_dump(uint8_t* buf, int buf_size) {
    int count = bkv_get_count(buf, buf_size);
    printf("%d kv for ", count);
    bkv_dump_buf("dump", buf, buf_size);

    for (int i = 0; i < count; i++) {
        int is_string_key = 0;
        char string_key[33];
        uint64_t number_key = 0;
        int result_get_key = bkv_get_key_by_index(buf, buf_size, i, &is_string_key, string_key, BKV_MAX_STRING_KEY_LEN, &number_key);
        if (result_get_key != 0) {
            printf("get key[%d] fail: %d", i, result_get_key);
            break;
        }
        if (is_string_key == 1) {
            printf("key-%d[s]:%16s -> ", i, string_key);
        } else {
            printf("key-%d[n]:%16lld -> ", i, number_key);
        }

        int value_pos_begin = 0;
        int value_pos_end = 0;
        int result_get_value = bkv_get_value_by_index(buf, buf_size, i, &value_pos_begin, &value_pos_end);
        if (result_get_value != 0) {
            printf("get value[%d] fail: %d", i, result_get_value);
            break;
        }

        bkv_dump_buf("value", buf + value_pos_begin, value_pos_end - value_pos_begin);
    }

    printf("\n");
}

void bkv_decode_string(const uint8_t* buf, size_t buf_size, char* value) {
    char value_buf[buf_size + 1];
    memcpy(value_buf, buf, buf_size);
    value_buf[buf_size] = 0;
    strcpy(value, value_buf);
}

int bkv_encode_float(float f, uint8_t* buf, int pos) {
    memcpy(buf, (uint8_t*)(&f), 4);
    if (!is_big_endian()) {
        bkv_reverse(buf, 4);
    }
    return pos + 4;
}

float bkv_decode_float(uint8_t* buf) {
    float f;
    uint8_t *fb = (uint8_t *) &f;
    memcpy(fb, buf, 4);
    if (!is_big_endian()) {
        bkv_reverse(fb, 4);
    }
    return f;
}

int bkv_encode_double(double f, uint8_t* buf, int pos) {
    memcpy(buf, (uint8_t*)(&f), 8);
    if (!is_big_endian()) {
        bkv_reverse(buf, 8);
    }
    return pos + 4;
}

double bkv_decode_double(uint8_t* buf) {
    double f;
    uint8_t *fb = (uint8_t *) &f;
    memcpy(fb, buf, 8);
    if (!is_big_endian()) {
        bkv_reverse(fb, 8);
    }
    return f;
}

int bkv_append_number_value_by_string_key(uint8_t* buf, int buf_size, char* key, uint64_t value) {
    int key_len = (int) strlen(key);
    int is_string_key = 1;

    uint8_t value_buf[16];
    int value_len = bkv_encode_number(value, value_buf, 0);

    return bkv_append(buf, buf_size, (uint8_t*)key, key_len, is_string_key, value_buf, value_len);
}

int bkv_append_number_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, uint64_t value) {
    uint8_t key_buf[16];
    int key_len = bkv_encode_number(key, key_buf, 0);

    uint8_t value_buf[16];
    int value_len = bkv_encode_number(value, value_buf, 0);

    return bkv_append(buf, buf_size, key_buf, key_len, 0, value_buf, value_len);
}

int bkv_append_string_value_by_string_key(uint8_t* buf, int buf_size, char* key, char* value) {
    int key_len = (int) strlen(key);

    int value_len = (int) strlen(value);

    return bkv_append(buf, buf_size, (uint8_t*)key, key_len, 1, (uint8_t*)value, value_len);
}

int bkv_append_string_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, char* value) {
    uint8_t key_buf[16];
    int key_len = bkv_encode_number(key, key_buf, 0);

    int value_len = (int) strlen(value);

    return bkv_append(buf, buf_size, key_buf, key_len, 0, (uint8_t*)value, value_len);
}

int bkv_append_float_value_by_string_key(uint8_t* buf, int buf_size, char* key, float value) {
    int key_len = (int) strlen(key);

    uint8_t value_buf[4];
    bkv_encode_float(value, value_buf, 0);

    return bkv_append(buf, buf_size, (uint8_t*)key, key_len, 1, value_buf, 4);
}

int bkv_append_float_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, float value) {
    uint8_t key_buf[16];
    int key_len = bkv_encode_number(key, key_buf, 0);

    uint8_t value_buf[4];
    bkv_encode_float(value, value_buf, 0);

    return bkv_append(buf, buf_size, key_buf, key_len, 0, value_buf, 4);
}

int bkv_append_double_value_by_string_key(uint8_t* buf, int buf_size, char* key, double value) {
    int key_len = (int) strlen(key);

    uint8_t value_buf[8];
    bkv_encode_double(value, value_buf, 0);

    return bkv_append(buf, buf_size, (uint8_t*)key, key_len, 1, value_buf, 8);
}

int bkv_append_double_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, double value) {
    uint8_t key_buf[16];
    int key_len = bkv_encode_number(key, key_buf, 0);

    uint8_t value_buf[8];
    bkv_encode_double(value, value_buf, 0);

    return bkv_append(buf, buf_size, key_buf, key_len, 0, value_buf, 8);
}

bkv_bool bkv_contains_string_key(uint8_t* buf, int buf_size, char* key) {
    return bkv_contains_key(buf, buf_size, key, 0, 1);
}

bkv_bool bkv_contains_number_key(uint8_t* buf, int buf_size, uint64_t key) {
    return bkv_contains_key(buf, buf_size, NULL, key, 0);
}

int bkv_get_count_by_key(uint8_t* buf, int buf_size, char* string_key, uint64_t number_key, int is_string_key) {
    int count = 0;
    int size = bkv_get_count(buf, buf_size);
    for (int i = 0 ; i < size; i++) {
        int item_is_string_key = 0;
        char item_string_key[BKV_MAX_STRING_KEY_LEN + 1];
        uint64_t item_number_key = 0;
        int result_get_key = bkv_get_key_by_index(buf, buf_size, i, &item_is_string_key, item_string_key, BKV_MAX_STRING_KEY_LEN, &item_number_key);
        if (result_get_key != BKV_RESULT_CODE_SUCCESS) {
            continue;
        }

        if (is_string_key == 1 && item_is_string_key == 1) {
            if (strcmp(item_string_key, string_key) == 0) {
                count++;
            }
        }

        if (is_string_key == 0 && item_is_string_key == 0) {
            if (item_number_key == number_key) {
                count++;
            }
        }
    }

    return count;
}

int bkv_get_key_value_by_index(uint8_t* buf, int buf_size, int index, int* is_string_key, char* string_key, int max_string_len, uint64_t* number_key, int* value_pos_begin, int* value_pos_end) {
    int kv_pos_begin = 0;
    int kv_pos_end = 0;
    int result_get_kv = bkv_get_kv_by_index(buf, buf_size, index, &kv_pos_begin, &kv_pos_end);
    if (result_get_kv != BKV_RESULT_CODE_SUCCESS) {
        // get kv fail
        return BKV_RESULT_CODE_GET_KV_FAIL;
    }

    if (kv_pos_end - kv_pos_begin <= 3) {
        // invalid kv
        return BKV_RESULT_CODE_KV_INVALID;
    }

    // LOGI("kv %d -> %d", kv_pos_begin, kv_pos_end);
    // bkv_dump_buf("kv", buf + kv_pos_begin, kv_pos_end - kv_pos_begin);

    int result_get_key_from_kv = bkv_get_key_from_kv(buf + kv_pos_begin, kv_pos_end - kv_pos_begin, is_string_key, string_key, max_string_len, number_key);
    if (result_get_key_from_kv != BKV_RESULT_CODE_SUCCESS) {
        // get key from kv fail
        return BKV_RESULT_CODE_GET_KEY_FAIL;
    }

    int kv_value_pos_begin = 0;
    int result_get_value_from_kv = bkv_get_value_from_kv(buf + kv_pos_begin, kv_pos_end - kv_pos_begin, &kv_value_pos_begin);
    if (result_get_value_from_kv != BKV_RESULT_CODE_SUCCESS) {
        // get value from kv fail
        return BKV_RESULT_CODE_GET_VALUE_FAIL;
    }

    *value_pos_begin = kv_pos_begin + kv_value_pos_begin;
    *value_pos_end = kv_pos_end;

    return BKV_RESULT_CODE_SUCCESS;
}

int bkv_get_key_by_index(uint8_t* buf, int buf_size, int index, int* is_string_key, char* string_key, int max_string_len, uint64_t* number_key) {
    int kv_pos_begin = 0;
    int kv_pos_end = 0;
    int result_get_kv = bkv_get_kv_by_index(buf, buf_size, index, &kv_pos_begin, &kv_pos_end);
    if (result_get_kv != BKV_RESULT_CODE_SUCCESS) {
        // get kv fail
        return BKV_RESULT_CODE_GET_KV_FAIL;
    }

    if (kv_pos_end - kv_pos_begin <= 3) {
        // invalid kv
        return BKV_RESULT_CODE_KV_INVALID;
    }

    // LOGI("kv %d -> %d", kv_pos_begin, kv_pos_end);
    // bkv_dump_buf("kv", buf + kv_pos_begin, kv_pos_end - kv_pos_begin);

    int result_get_key_from_kv = bkv_get_key_from_kv(buf + kv_pos_begin, kv_pos_end - kv_pos_begin, is_string_key, string_key, max_string_len, number_key);
    if (result_get_key_from_kv != BKV_RESULT_CODE_SUCCESS) {
        // get key from kv fail
        return BKV_RESULT_CODE_GET_KEY_FAIL;
    }

    return BKV_RESULT_CODE_SUCCESS;
}

int bkv_get_value_by_index(uint8_t* buf, int buf_size, int index, int* pos_begin, int* pos_end) {
    int kv_pos_begin = 0;
    int kv_pos_end = 0;
    int result_get_kv = bkv_get_kv_by_index(buf, buf_size, index, &kv_pos_begin, &kv_pos_end);
    if (result_get_kv != BKV_RESULT_CODE_SUCCESS) {
        // get kv fail
        return BKV_RESULT_CODE_GET_KV_FAIL;
    }

    if (kv_pos_end - kv_pos_begin <= 3) {
        // invalid kv
        return BKV_RESULT_CODE_KV_INVALID;
    }

    int kv_value_pos_begin = 0;
    int result_get_value_from_kv = bkv_get_value_from_kv(buf + kv_pos_begin, kv_pos_end - kv_pos_begin, &kv_value_pos_begin);
    if (result_get_value_from_kv != BKV_RESULT_CODE_SUCCESS) {
        // get value from kv fail
        return BKV_RESULT_CODE_GET_VALUE_FAIL;
    }

    *pos_begin = kv_pos_begin + kv_value_pos_begin;
    *pos_end = kv_pos_end;

    return BKV_RESULT_CODE_SUCCESS;
}

bkv_bool bkv_contains_key(uint8_t* buf, int buf_size, char* string_key, uint64_t number_key, int is_string_key) {
    int count = bkv_get_count(buf, buf_size);
    for (int i = 0; i < count; i++) {
        int item_is_string_key = 0;
        char item_string_key[BKV_MAX_STRING_KEY_LEN + 1];
        uint64_t item_number_key = 0;
        int result_get_key = bkv_get_key_by_index(buf, buf_size, i, &item_is_string_key, item_string_key, BKV_MAX_STRING_KEY_LEN, &item_number_key);
        if (result_get_key != BKV_RESULT_CODE_SUCCESS) {
            continue;
        }

        if (is_string_key == 1 && item_is_string_key == 1) {
            if (strcmp(item_string_key, string_key) == 0) {
                return BKV_TRUE;
            }
        }

        if (is_string_key == 0 && item_is_string_key == 0) {
            if (item_number_key == number_key) {
                return BKV_TRUE;
            }
        }
    }

    return BKV_FALSE;
}



int bkv_get_value_by_key(uint8_t* buf, int buf_size, int* value_pos_begin, int* value_pos_end, char* string_key, uint64_t number_key, int is_string_key) {
    int count = bkv_get_count(buf, buf_size);
    for (int i = 0; i < count; i++) {
        int item_is_string_key = 0;
        char item_string_key[BKV_MAX_STRING_KEY_LEN + 1];
        uint64_t item_number_key = 0;
        int result_get_key = bkv_get_key_by_index(buf, buf_size, i, &item_is_string_key, item_string_key, BKV_MAX_STRING_KEY_LEN, &item_number_key);
        if (result_get_key != BKV_RESULT_CODE_SUCCESS) {
            continue;
        }

        int found = 0;
        if (item_is_string_key == 1 && is_string_key == 1) {
            if (strcmp(item_string_key, string_key) == 0) {
                found = 1;
            }
        }

        if (item_is_string_key == 0 && is_string_key == 0) {
            if (item_number_key == number_key) {
                found = 1;
            }
        }

        if (found) {
            int result_get_value = bkv_get_value_by_index(buf, buf_size, i, value_pos_begin, value_pos_end);
            if (result_get_value == BKV_RESULT_CODE_SUCCESS) {
                return BKV_RESULT_CODE_SUCCESS;
            }
        }
    }

    return BKV_RESULT_CODE_FAIL;
}

int bkv_get_value_by_string_key(uint8_t* buf, int buf_size, char* key, int* value_pos_begin, int* value_pos_end) {
    return bkv_get_value_by_key(buf, buf_size, value_pos_begin, value_pos_end, key, 0, 1);
}

int bkv_get_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, int* value_pos_begin, int* value_pos_end) {
    return bkv_get_value_by_key(buf, buf_size, value_pos_begin, value_pos_end, NULL, key, 0);
}

int bkv_get_number_value_by_key(uint8_t* buf, int buf_size, uint64_t* value, char* string_key, uint64_t number_key, int is_string_key) {
    if (bkv_contains_key(buf, buf_size, string_key, number_key, is_string_key) == BKV_FALSE) {
        return BKV_RESULT_CODE_GET_KEY_FAIL;
    }

    int value_pos_begin = 0;
    int value_pos_end = 0;
    int result_code = bkv_get_value_by_key(buf, buf_size, &value_pos_begin, &value_pos_end, string_key, number_key, is_string_key);
    if (result_code != 0) {
        return BKV_RESULT_CODE_GET_VALUE_FAIL;
    }

    *value = bkv_decode_number(buf + value_pos_begin, value_pos_end - value_pos_begin);

    return BKV_RESULT_CODE_SUCCESS;
}

int bkv_get_number_value_by_string_key(uint8_t* buf, int buf_size, char* key, uint64_t* value) {
    return bkv_get_number_value_by_key(buf, buf_size, value, key, 0, 1);
}

int bkv_get_number_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, uint64_t* value) {
    return bkv_get_number_value_by_key(buf, buf_size, value, NULL, key, 0);
}

int bkv_get_string_value_by_key(uint8_t* buf, int buf_size, char* value, char* string_key, uint64_t number_key, int is_string_key) {
    if (bkv_contains_key(buf, buf_size, string_key, number_key, is_string_key) == BKV_FALSE) {
        return BKV_RESULT_CODE_GET_KEY_FAIL;
    }

    int value_pos_begin = 0;
    int value_pos_end = 0;
    int result_code = bkv_get_value_by_key(buf, buf_size, &value_pos_begin, &value_pos_end, string_key, number_key, is_string_key);
    if (result_code != 0) {
        return BKV_RESULT_CODE_GET_VALUE_FAIL;
    }

    int value_len = value_pos_end - value_pos_begin;
    char value_buf[value_len + 1];
    memcpy(value_buf, buf + value_pos_begin, value_pos_end - value_pos_begin);
    value_buf[value_len] = 0;

    strcpy(value, value_buf);

    return BKV_RESULT_CODE_SUCCESS;
}

int bkv_get_string_value_by_string_key(uint8_t* buf, int buf_size, char* key, char* value) {
    return bkv_get_string_value_by_key(buf, buf_size, value, key, 0, 1);
}

int bkv_get_string_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, char* value) {
    return bkv_get_string_value_by_key(buf, buf_size, value, NULL, key, 0);
}

int bkv_get_float_value_by_key(uint8_t* buf, int buf_size, float* value, char* string_key, uint64_t number_key, int is_string_key) {
    if (bkv_contains_key(buf, buf_size, string_key, number_key, is_string_key) == BKV_FALSE) {
        return BKV_RESULT_CODE_GET_KEY_FAIL;
    }

    int value_pos_begin = 0;
    int value_pos_end = 0;
    int result_code = bkv_get_value_by_key(buf, buf_size, &value_pos_begin, &value_pos_end, string_key, number_key, is_string_key);
    if (result_code != 0) {
        return BKV_RESULT_CODE_GET_VALUE_FAIL;
    }

    *value = bkv_decode_float(buf + value_pos_begin);

    return BKV_RESULT_CODE_SUCCESS;
}

int bkv_get_float_value_by_string_key(uint8_t* buf, int buf_size, char* key, float* value) {
    return bkv_get_float_value_by_key(buf, buf_size, value, key, 0, 1);
}

int bkv_get_float_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, float* value) {
    return bkv_get_float_value_by_key(buf, buf_size, value, NULL, key, 0);
}

int bkv_get_number_value_list_by_key(uint8_t* buf, int buf_size, uint64_t* list, char* string_key, uint64_t number_key, int is_string_key) {
    int count = 0;
    int size = bkv_get_count(buf, buf_size);
    for (int i = 0; i < size; i++) {
        int item_is_string_key = 0;
        char item_string_key[BKV_MAX_STRING_KEY_LEN + 1];
        uint64_t item_number_key = 0;
        int value_pos_begin = 0;
        int value_pos_end = 0;
        int result_get_key = bkv_get_key_value_by_index(buf, buf_size, i, &item_is_string_key, item_string_key, BKV_MAX_STRING_KEY_LEN, &item_number_key, &value_pos_begin, &value_pos_end);
        if (result_get_key != BKV_RESULT_CODE_SUCCESS) {
            continue;
        }

        int found = 0;
        if (item_is_string_key == 1 && is_string_key == 1) {
            if (strcmp(item_string_key, string_key) == 0) {
                found = 1;
            }
        }

        if (item_is_string_key == 0 && is_string_key == 0) {
            if (item_number_key == number_key) {
                found = 1;
            }
        }

        if (found) {
            uint64_t value = bkv_decode_number(buf + value_pos_begin, value_pos_end - value_pos_begin);
            list[count++] = value;
        }
    }

    return count;
}

int bkv_get_number_value_list_by_string_key(uint8_t* buf, int buf_size, uint64_t* list, char* key) {
    return bkv_get_number_value_list_by_key(buf, buf_size, list, key, 0, 1);
}

int bkv_get_number_value_list_by_number_key(uint8_t* buf, int buf_size, uint64_t* list, uint64_t key) {
    return bkv_get_number_value_list_by_key(buf, buf_size, list, NULL, key, 0);
}

int bkv_traverse(uint8_t* buf, int buf_size, void (*func)(int is_string_key, char* string_key, uint64_t* number_key, const uint8_t* value, int value_len, void* data), void* func_data) {
    int count = 0;
    int remaining_size = buf_size;
    int pos = 0;

    while (1) {
        int result_code = 0;
        uint64_t length = 0;
        int length_bytes_size = 0;
        bkv_decode_length(buf + pos, remaining_size, &result_code, &length, &length_bytes_size);
        if (result_code != 0 || length <= 0 || length_bytes_size <= 0) {
            // decode length error
            return BKV_RESULT_CODE_FAIL;
        }
        int payload_len = length_bytes_size + (int)length;
        remaining_size -= payload_len;
        if (remaining_size < 0) {
            return BKV_RESULT_CODE_FAIL;
        }

        int is_string_key = 0;
        char string_key[BKV_MAX_STRING_KEY_LEN + 1];
        uint64_t number_key = 0;
        int get_key_result = bkv_get_key_from_kv(buf + pos, payload_len, &is_string_key, string_key, BKV_MAX_STRING_KEY_LEN, &number_key);
        if (get_key_result != 0) {
            return BKV_RESULT_CODE_FAIL;
        }
        int value_pos_begin = 0;
        int get_value_result = bkv_get_value_from_kv(buf + pos, payload_len, &value_pos_begin);
        if (get_value_result != 0) {
            return BKV_RESULT_CODE_FAIL;
        }

        func(is_string_key, string_key, &number_key, buf + pos + value_pos_begin, payload_len - value_pos_begin, func_data);

        if (remaining_size == 0) {
            // complete
            return 0;
        }

        pos += payload_len;
        count++;
    }
}







int bkv_context_append(struct bkv_context* context, const uint8_t* key, int key_len, int is_string_key, const uint8_t* value, int value_len) {
    uint8_t* buf = context->buf + context->offset;
    int buf_size = context->size - context->offset;
    int offset = bkv_append(buf, buf_size, key, key_len, is_string_key, value, value_len);
    if (offset <= 0) {
        return offset;
    }

    context->offset += offset;
    return 0;
}

int bkv_context_append_number_value_by_string_key(struct bkv_context* context, char* key, uint64_t value) {
    uint8_t* buf = context->buf + context->offset;
    int buf_size = context->size - context->offset;
    int offset = bkv_append_number_value_by_string_key(buf, buf_size, key, value);
    if (offset <= 0) {
        return offset;
    }

    context->offset += offset;
    return 0;
}

int bkv_context_append_number_value_by_number_key(struct bkv_context* context, uint64_t key, uint64_t value) {
    uint8_t* buf = context->buf + context->offset;
    int buf_size = context->size - context->offset;
    int offset = bkv_append_number_value_by_number_key(buf, buf_size, key, value);
    if (offset <= 0) {
        return offset;
    }

    context->offset += offset;
    return 0;
}

int bkv_context_append_string_value_by_string_key(struct bkv_context* context, char* key, char* value) {
    uint8_t* buf = context->buf + context->offset;
    int buf_size = context->size - context->offset;
    int offset = bkv_append_string_value_by_string_key(buf, buf_size, key, value);
    if (offset <= 0) {
        return offset;
    }

    context->offset += offset;
    return 0;
}

int bkv_context_append_string_value_by_number_key(struct bkv_context* context, uint64_t key, char* value) {
    uint8_t* buf = context->buf + context->offset;
    int buf_size = context->size - context->offset;
    int offset = bkv_append_string_value_by_number_key(buf, buf_size, key, value);
    if (offset <= 0) {
        return offset;
    }

    context->offset += offset;
    return 0;
}

int bkv_context_append_float_value_by_string_key(struct bkv_context* context, char* key, float value) {
    uint8_t* buf = context->buf + context->offset;
    int buf_size = context->size - context->offset;
    int offset = bkv_append_float_value_by_string_key(buf, buf_size, key, value);
    if (offset <= 0) {
        return offset;
    }

    context->offset += offset;
    return 0;
}

int bkv_context_append_float_value_by_number_key(struct bkv_context* context, uint64_t key, float value) {
    uint8_t* buf = context->buf + context->offset;
    int buf_size = context->size - context->offset;
    int offset = bkv_append_float_value_by_number_key(buf, buf_size, key, value);
    if (offset <= 0) {
        return offset;
    }

    context->offset += offset;
    return 0;
}

int bkv_context_append_double_value_by_string_key(struct bkv_context* context, char* key, double value) {
    uint8_t* buf = context->buf + context->offset;
    int buf_size = context->size - context->offset;
    int offset = bkv_append_double_value_by_string_key(buf, buf_size, key, value);
    if (offset <= 0) {
        return offset;
    }

    context->offset += offset;
    return 0;
}

int bkv_context_append_double_value_by_number_key(struct bkv_context* context, uint64_t key, double value) {
    uint8_t* buf = context->buf + context->offset;
    int buf_size = context->size - context->offset;
    int offset = bkv_append_double_value_by_number_key(buf, buf_size, key, value);
    if (offset <= 0) {
        return offset;
    }

    context->offset += offset;
    return 0;
}

int bkv_context_get_count(struct bkv_context* context) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_count(buf, buf_size);
}

int bkv_context_get_count_by_key(struct bkv_context* context, char* string_key, uint64_t number_key, int is_string_key) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_count_by_key(buf, buf_size, string_key, number_key, is_string_key);
}

int bkv_context_get_kv_by_index(struct bkv_context* context, int index, int* pos_begin, int* pos_end) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_kv_by_index(buf, buf_size, index, pos_begin, pos_end);
}

int bkv_context_get_key_value_by_index(struct bkv_context* context, int index, int* is_string_key, char* string_key, int max_string_len, uint64_t* number_key, int* value_pos_begin, int* value_pos_end) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_key_value_by_index(buf, buf_size, index, is_string_key, string_key, max_string_len, number_key, value_pos_begin, value_pos_end);
}

int bkv_context_get_key_by_index(struct bkv_context* context, int index, int* is_string_key, char* string_key, int max_string_len, uint64_t* number_key) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_key_by_index(buf, buf_size, index, is_string_key, string_key, max_string_len, number_key);
}

int bkv_context_get_value_by_index(struct bkv_context* context, int index, int* pos_begin, int* pos_end) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_value_by_index(buf, buf_size, index, pos_begin, pos_end);
}

bkv_bool bkv_context_contains_key(struct bkv_context* context, char* string_key, uint64_t number_key, int is_string_key) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_contains_key(buf, buf_size, string_key, number_key, is_string_key);
}

bkv_bool bkv_context_contains_string_key(struct bkv_context* context, char* key) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_contains_string_key(buf, buf_size, key);
}

bkv_bool bkv_context_contains_number_key(struct bkv_context* context, uint64_t key) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_contains_number_key(buf, buf_size, key);
}

int bkv_context_get_value_by_key(struct bkv_context* context, int* value_pos_begin, int* value_pos_end, char* string_key, uint64_t number_key, int is_string_key) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_value_by_key(buf, buf_size, value_pos_begin, value_pos_end, string_key, number_key, is_string_key);
}

int bkv_context_get_value_by_string_key(struct bkv_context* context, char* key, int* value_pos_begin, int* value_pos_end) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_value_by_string_key(buf, buf_size, key, value_pos_begin, value_pos_end);
}

int bkv_context_get_value_by_number_key(struct bkv_context* context, uint64_t key, int* value_pos_begin, int* value_pos_end) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_value_by_number_key(buf, buf_size, key, value_pos_begin, value_pos_end);
}

int bkv_context_get_number_value_by_key(struct bkv_context* context, uint64_t* value, char* string_key, uint64_t number_key, int is_string_key) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_number_value_by_key(buf, buf_size, value, string_key, number_key, is_string_key);
}

int bkv_context_get_number_value_by_string_key(struct bkv_context* context, char* key, uint64_t* value) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_number_value_by_string_key(buf, buf_size, key, value);
}

int bkv_context_get_number_value_by_number_key(struct bkv_context* context, uint64_t key, uint64_t* value) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_number_value_by_number_key(buf, buf_size, key, value);
}

int bkv_context_get_string_value_by_key(struct bkv_context* context, char* value, char* string_key, uint64_t number_key, int is_string_key) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_string_value_by_key(buf, buf_size, value, string_key, number_key, is_string_key);
}

int bkv_context_get_string_value_by_string_key(struct bkv_context* context, char* key, char* value) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_string_value_by_string_key(buf, buf_size, key, value);
}

int bkv_context_get_string_value_by_number_key(struct bkv_context* context, uint64_t key, char* value) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_string_value_by_number_key(buf, buf_size, key, value);
}

int bkv_context_get_float_value_by_key(struct bkv_context* context, float* value, char* string_key, uint64_t number_key, int is_string_key) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_float_value_by_key(buf, buf_size, value, string_key, number_key, is_string_key);
}

int bkv_context_get_float_value_by_string_key(struct bkv_context* context, char* key, float* value) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_float_value_by_string_key(buf, buf_size, key, value);
}

int bkv_context_get_float_value_by_number_key(struct bkv_context* context, uint64_t key, float* value) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_float_value_by_number_key(buf, buf_size, key, value);
}

int bkv_context_get_number_value_list_by_key(struct bkv_context* context, uint64_t* list, char* string_key, uint64_t number_key, int is_string_key) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_number_value_list_by_key(buf, buf_size, list, string_key, number_key, is_string_key);
}

int bkv_context_get_number_value_list_by_string_key(struct bkv_context* context, uint64_t* list, char* key) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_number_value_list_by_string_key(buf, buf_size, list, key);
}

int bkv_context_get_number_value_list_by_number_key(struct bkv_context* context, uint64_t* list, uint64_t key) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_get_number_value_list_by_number_key(buf, buf_size, list, key);
}

int bkv_context_traverse(struct bkv_context* context, void (*func)(int is_string_key, char* string_key, uint64_t* number_key, const uint8_t* value, int value_len, void* data), void* func_data) {
    uint8_t* buf = context->buf;
    int buf_size = context->size;
    return bkv_traverse(buf, buf_size, func, func_data);
}