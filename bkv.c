#include "bkv.h"

void dump_buf(char* name, u_int8_t* buf, int buf_size) {
    int i, j;

    printf("%-5s[%d]: ", name, buf_size);
    for (i = 0; i < buf_size; i++) {
        printf("%02X", *(buf + i));
    }

    // check printable string
    if (buf_size != 0) {
        u_int8_t first_byte = *(buf);
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

void dump_bkv(u_int8_t* buf, int buf_size) {
    int count = bkv_get_count(buf, buf_size);
    printf("%d kv for ", count);
    dump_buf("dump", buf, buf_size);

    for (int i = 0; i < count; i++) {
        int is_string_key = 0;
        char string_key[33];
        u_int64_t number_key = 0;
        int result_get_key = bkv_get_key_by_index(buf, buf_size, i, &is_string_key, string_key, 32, &number_key);
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

        dump_buf("value", buf + value_pos_begin, value_pos_end - value_pos_begin);
    }

    printf("\n");
}

void reverse(u_int8_t* bs, size_t size) {
    int i, j;
    for (i = 0, j = size - 1; i < j; i++, j--) {
        u_int8_t tmp = *(bs + i);
        *(bs + i) = *(bs + j);
        *(bs + j) = tmp;
    }
}

int encode_number(u_int64_t number, u_int8_t* buf, int pos) {
    u_int8_t nb[16]; 
    if (number == 0) {
        nb[0] = 0;
        return pos;
    }

    int i = 0;
    while (number > 0) {
        nb[i] = number & 0xFF;
        number >>= 8;
        i++;
    }

    reverse(nb, i);

    for (int j = 0; j < i; j++) {
        *(buf + j) = nb[j];
    }

    return pos + i;
}

u_int64_t decode_number(u_int8_t* buf, size_t buf_size) {
    int i;

    if (buf_size > 8) {
        buf_size = 8;
    }
    u_int64_t n = 0;
    for (i = 0; i < buf_size; i++) {
        n <<= 8;
        n |= buf[i];
    }

    return n;
}

int get_length_encoded_size(u_int64_t length) {
    int i = 0;
    while (length > 0) {
        length >>= 7;
        i++;
    }    
    return i;
}

int encode_length(u_int64_t length, u_int8_t* buf, int pos) {
    u_int8_t nb[16]; 
    int i = 0;
    while (length > 0) {
        nb[i] = (length & 0x7F) | 0x80;
        length >>= 7;
        i++;
    }    

    reverse(nb, i);
    nb[i - 1] &= 0x7F;

    for (int j = 0; j < i; j++) {
        *(buf + pos + j) = nb[j];
    }    

    return pos + i;
}

void decode_length(u_int8_t* buf, size_t buf_size, int* result_code, u_int64_t* result_length, int* result_length_byte_size) {
    // single byte
    u_int8_t first_byte = *buf;
    if ((first_byte & 0x80) == 0) {
        *result_code = 0;
        *result_length = first_byte;
        *result_length_byte_size = 1;
        return ;          
    }

    // multiple bytes
    int length_byte_size = 0;
    int i;
    for (i = 0; i < buf_size; i++) {
        length_byte_size++;
        if ((*(buf + i) & 0x80) == 0) {
            break;
        }
    }

    if (length_byte_size == 0 || length_byte_size > 4) {
        // error: invalid length buf
        LOGE("invalid length buf");
        *result_code = -1;
        return ;
    }

    u_int64_t length = 0;

    for (i = 0; i < length_byte_size; i++) {
        length <<= 7;
        length |= *(buf + i) & 0x7F;
    }

    *result_code = 0;
    *result_length = length;
    *result_length_byte_size = length_byte_size;
}

// encode related functions

int bkv_append_by_string_key(u_int8_t* buf, int buf_size, int pos, char* key, u_int8_t* value, int value_len) {
    int key_len = strlen(key);
    int is_string_key = 1;
    return bkv_append(buf, buf_size, pos, (u_int8_t*)key, key_len, is_string_key, value, value_len);
}

int bkv_append_by_number_key(u_int8_t* buf, int buf_size, int pos, u_int64_t key, u_int8_t* value, int value_len) {
    u_int8_t key_buf[16];
    int key_len = encode_number(key, key_buf, 0);
    return bkv_append(buf, buf_size, pos, key_buf, key_len, 0, value, value_len);
}

int bkv_append(u_int8_t* buf, int buf_size, int pos, u_int8_t* key, int key_len, int is_string_key, u_int8_t* value, int value_len) {
    u_int64_t payload_length = key_len + 1 + value_len;
    int length_encoded_size = get_length_encoded_size(payload_length);
    if (pos + length_encoded_size + payload_length >= buf_size) {
        // length not enough
        return -1;
    }

    // append len
    int p = encode_length(payload_length, buf, pos);

    // append key length byte
    u_int8_t key_lenght_byte = key_len & 0x7F;
    if (is_string_key != 0) {
        key_lenght_byte |= 0x80;
    }
    *(buf + p) = key_lenght_byte;
    p += 1;

    // append key buf
    int i = 0;
    for (; i < key_len; i++) {
        *(buf + p + i) = key[i];
    }
    p += key_len;

    // append value buf
    for (i = 0; i < value_len; i++) {
        *(buf + p + i) = value[i];
    }
    p += value_len;  

    return p;
}


// decode related functions

int bkv_get_count(u_int8_t* buf, int buf_size) {
    int count = 0;
    u_int8_t* p = buf;
    int remaining_size = buf_size;
    int pos = 0;

    while (1) {
        int result_code = 0;
        u_int64_t result_length = 0;
        int result_length_bytes_size = 0;
        decode_length(buf + pos, remaining_size, &result_code, &result_length, &result_length_bytes_size);
        if (result_code != 0 || result_length <= 0 || result_length_bytes_size <= 0) {
            // decode length error
            return -1;
        }
        int payload_len = result_length_bytes_size + result_length;
        pos += payload_len;
        remaining_size -= payload_len;
        count++;
        if (remaining_size == 0) {
            return count;
        }
        if (remaining_size < 0) {
            return -1;
        }
    }
}

int bkv_get_kv_by_index(u_int8_t* buf, int buf_size, int index, int* pos_begin, int* pos_payload_begin, int* pos_end) {
    int count = 0;
    u_int8_t* p = buf;
    int remaining_size = buf_size;
    int pos = 0;

    while (1) {
        int result_code = 0;
        u_int64_t result_length = 0;
        int result_length_bytes_size = 0;
        decode_length(buf + pos, remaining_size, &result_code, &result_length, &result_length_bytes_size);
        if (result_code != 0 || result_length <= 0 || result_length_bytes_size <= 0) {
            // decode length error
            return -1;
        }
        int payload_len = result_length_bytes_size + result_length;
        remaining_size -= payload_len;
        if (remaining_size < 0) {
            return -1;
        }

        if (count == index) {
            *pos_begin = pos;
            *pos_payload_begin = pos + result_length_bytes_size;
            *pos_end = pos + payload_len;
            return 0;
        }

        if (remaining_size == 0) {
            // can not find one till to the end
            return -1;
        }

        pos += payload_len;
        count++;
    }
}

int bkv_get_key_from_kv_payload(u_int8_t* buf, int buf_size, int* is_string_key, char* string_key, int max_string_len, u_int64_t* number_key) {
    if (buf_size <= 0) {
        // invalid buf
        return -1;
    }

    u_int8_t first_byte = *buf;
    *is_string_key = 0;
    if ((first_byte & 0x80) != 0) {
        *is_string_key = 1;
    }

    int key_len = first_byte & 0x7F;
    if (key_len > max_string_len) {
        // key len exceeds string len
        return -2;
    }
    if (key_len + 1 > buf_size) {
        // key len exceeds buf len
        return -3;
    }

    if (*is_string_key == 1) {
        // strcpy
        for (int i = 0; i < key_len; i++) {
            *(string_key + i) = (char) (*(buf + 1 + i));
        }
        *(string_key + key_len) = 0;
    } else {
        *number_key = decode_number(buf + 1, key_len);
    }

    return 0;
}

int bkv_get_value_from_kv_payload(u_int8_t* buf, int buf_size, int* pos_begin) {
    if (buf_size <= 0) {
        // invalid buf
        return -1;
    }

    u_int8_t first_byte = *buf;
    int key_len = first_byte & 0x7F;

    if (key_len + 1 > buf_size) {
        // key len exceeds buf len
        return -2;
    }    

    *pos_begin = 1 + key_len;

    return 0;
}

int bkv_get_key_by_index(u_int8_t* buf, int buf_size, int index, int* is_string_key, char* string_key, int max_string_len, u_int64_t* number_key) {
    int kv_pos_begin = 0;
    int kv_pos_payload_begin = 0;
    int kv_pos_end = 0;
    int result_get_kv = bkv_get_kv_by_index(buf, buf_size, index, &kv_pos_begin, &kv_pos_payload_begin, &kv_pos_end);
    if (result_get_kv != 0) {
        // get kv fail
        return -1;
    }

    if (kv_pos_end - kv_pos_begin <= 2) {
        // invalid kv
        return -2;
    }    

    // LOGI("kv %d -> %d", kv_pos_begin, kv_pos_end);
    // dump_buf("kv", buf + kv_pos_begin, kv_pos_end - kv_pos_begin);

    int result_get_key_from_kv = bkv_get_key_from_kv_payload(buf + kv_pos_payload_begin, kv_pos_end - kv_pos_payload_begin, is_string_key, string_key, max_string_len, number_key);
    if (result_get_key_from_kv != 0) {
        // get key from kv fail
        return -2;
    }

    return 0;
}

int bkv_get_value_by_index(u_int8_t* buf, int buf_size, int index, int* pos_begin, int* pos_end) {
    int kv_pos_begin = 0;
    int kv_pos_payload_begin = 0;
    int kv_pos_end = 0;
    int result_get_kv = bkv_get_kv_by_index(buf, buf_size, index, &kv_pos_begin, &kv_pos_payload_begin, &kv_pos_end);
    if (result_get_kv != 0) {
        // get kv fail
        return -1;
    }

    if (kv_pos_end - kv_pos_begin <= 2) {
        // invalid kv
        return -2;
    }    

    int kv_value_pos_begin = 0;
    int result_get_value_from_kv = bkv_get_value_from_kv_payload(buf + kv_pos_payload_begin, kv_pos_end - kv_pos_payload_begin, &kv_value_pos_begin);
    if (result_get_value_from_kv != 0) {
        // get value from kv fail
        return -3;
    }

    *pos_begin = kv_pos_payload_begin + kv_value_pos_begin;
    *pos_end = kv_pos_end;

    return 0;
}

int bkv_contains_string_key(u_int8_t* buf, int buf_size, char* key) {
    int count = bkv_get_count(buf, buf_size);
    for (int i = 0; i < count; i++) {
        int is_string_key = 0;
        char string_key[BKV_MAX_STRING_KEY_LEN + 1];
        u_int64_t number_key = 0;
        int result_get_key = bkv_get_key_by_index(buf, buf_size, i, &is_string_key, string_key, BKV_MAX_STRING_KEY_LEN, &number_key);
        if (result_get_key != 0) {
            break;
        }
        if (is_string_key == 1) {
            if (strcmp(key, string_key) == 0) {
                return 1;
            }
        }
    }

    return 0;
}

int bkv_contains_number_key(u_int8_t* buf, int buf_size, u_int64_t key) {
    int count = bkv_get_count(buf, buf_size);
    for (int i = 0; i < count; i++) {
        int is_string_key = 0;
        char string_key[33];
        u_int64_t number_key = 0;
        int result_get_key = bkv_get_key_by_index(buf, buf_size, i, &is_string_key, string_key, 32, &number_key);
        if (result_get_key != 0) {
            return 0;
        }
        if (is_string_key == 0) {
            if (number_key == key) {
                return 1;
            }
        } 
    }    

    return 0;
}

int bkv_get_value_by_string_key(u_int8_t* buf, int buf_size, char* key, int* value_pos_begin, int* value_pos_end) {
    int count = bkv_get_count(buf, buf_size);
    for (int i = 0; i < count; i++) {
        int is_string_key = 0;
        char string_key[BKV_MAX_STRING_KEY_LEN + 1];
        u_int64_t number_key = 0;
        int result_get_key = bkv_get_key_by_index(buf, buf_size, i, &is_string_key, string_key, BKV_MAX_STRING_KEY_LEN, &number_key);
        if (result_get_key != 0) {
            break;
        }
        if (is_string_key == 1) {
            if (strcmp(key, string_key) == 0) {
                int result_get_value = bkv_get_value_by_index(buf, buf_size, i, value_pos_begin, value_pos_end);
                if (result_get_value == 0) {
                    return 0;
                }
            }
        }
    }

    return -1;
}

int bkv_get_value_by_number_key(u_int8_t* buf, int buf_size, u_int64_t key, int* value_pos_begin, int* value_pos_end) {
    int count = bkv_get_count(buf, buf_size);
    for (int i = 0; i < count; i++) {
        int is_string_key = 0;
        char string_key[33];
        u_int64_t number_key = 0;
        int result_get_key = bkv_get_key_by_index(buf, buf_size, i, &is_string_key, string_key, 32, &number_key);
        if (result_get_key != 0) {
            return 0;
        }
        if (is_string_key == 0) {
            if (number_key == key) {
                int result_get_value = bkv_get_value_by_index(buf, buf_size, i, value_pos_begin, value_pos_end);
                if (result_get_value == 0) {
                    return 0;
                }
            }
        } 
    }    

    return 0;    
}