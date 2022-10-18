#include "bkv.h"

const int bkv_endian_i = 1;

void bkv_reverse(uint8_t* bs, size_t size) {
    int i, j;
    for (i = 0, j = (int)size - 1; i < j; i++, j--) {
        uint8_t tmp = *(bs + i);
        *(bs + i) = *(bs + j);
        *(bs + j) = tmp;
    }
}

int bkv_encode_number(uint64_t number, uint8_t* buf, int pos) {
    uint8_t nb[16];
    if (number == 0) {
        nb[0] = 0;
        buf[0] = 0;
        return pos + 1;
    }

    int i = 0;
    while (number > 0) {
        nb[i] = number & 0xFF;
        number >>= 8;
        i++;
    }

    bkv_reverse(nb, i);

    for (int j = 0; j < i; j++) {
        *(buf + j) = nb[j];
    }

    return pos + i;
}

uint64_t bkv_decode_number(const uint8_t* buf, size_t buf_size) {
    int i;

    if (buf_size > 8) {
        buf_size = 8;
    }
    uint64_t n = 0;
    for (i = 0; i < buf_size; i++) {
        n <<= 8;
        n |= buf[i];
    }

    return n;
}

int get_length_encoded_size(uint64_t length) {
    int i = 0;
    while (length > 0) {
        length >>= 7;
        i++;
    }
    return i;
}

int encode_length(uint64_t length, uint8_t* buf) {
    uint8_t nb[16];
    int i = 0;
    while (length > 0) {
        nb[i] = (uint8_t) ((length & 0x7F) | 0x80);
        length >>= 7;
        i++;
    }

    bkv_reverse(nb, i);
    nb[i - 1] &= 0x7F;

    for (int j = 0; j < i; j++) {
        *(buf + j) = nb[j];
    }

    return i;
}

void bkv_decode_length(const uint8_t* buf, size_t buf_size, int* result_code, uint64_t* result_length, int* result_length_byte_size) {
    // single byte
    uint8_t first_byte = *buf;
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
        // LOGE("invalid length buf");
        *result_code = -1;
        return ;
    }

    uint64_t length = 0;

    for (i = 0; i < length_byte_size; i++) {
        length <<= 7;
        length |= *(buf + i) & 0x7F;
    }

    *result_code = 0;
    *result_length = length;
    *result_length_byte_size = length_byte_size;
}

// encode related functions

int bkv_append_by_string_key(uint8_t* buf, int buf_size, char* key, uint8_t* value, int value_len) {
    int key_len = (int) strlen(key);
    int is_string_key = 1;
    return bkv_append(buf, buf_size, (uint8_t*)key, key_len, is_string_key, value, value_len);
}

int bkv_append_by_number_key(uint8_t* buf, int buf_size, uint64_t key, uint8_t* value, int value_len) {
    uint8_t key_buf[16];
    int key_len = bkv_encode_number(key, key_buf, 0);
    return bkv_append(buf, buf_size, key_buf, key_len, 0, value, value_len);
}

int bkv_append(uint8_t* buf, int buf_size, const uint8_t* key, int key_len, int is_string_key, const uint8_t* value, int value_len) {
    int payload_length = key_len + 1 + value_len;
    int length_encoded_size = get_length_encoded_size(payload_length);
    if (length_encoded_size + payload_length >= buf_size) {
        // length not enough
        return BKV_RESULT_CODE_FAIL;
    }

    // append len
    int p = encode_length(payload_length, buf);

    // append key length byte
    uint8_t key_length_byte = (uint8_t) (key_len & 0x7F);
    if (is_string_key != 0) {
        key_length_byte |= 0x80;
    }
    *(buf + p) = key_length_byte;
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

int bkv_get_count(uint8_t* buf, int buf_size) {
    int count = 0;
    int remaining_size = buf_size;
    int pos = 0;

    while (1) {
        int result_code = 0;
        uint64_t result_length = 0;
        int result_length_bytes_size = 0;
        bkv_decode_length(buf + pos, remaining_size, &result_code, &result_length, &result_length_bytes_size);
        if (result_code != 0 || result_length <= 0 || result_length_bytes_size <= 0) {
            // decode length error
            return -1;
        }
        int payload_len = result_length_bytes_size + (int)result_length;
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



int bkv_get_kv_by_index(uint8_t* buf, int buf_size, int index, int* pos_begin, int* pos_end) {
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

        if (count == index) {
            *pos_begin = pos;
            *pos_end = pos + payload_len;
            return BKV_RESULT_CODE_SUCCESS;
        }

        if (remaining_size == 0) {
            // can not find one till to the end
            return BKV_RESULT_CODE_FAIL;
        }

        pos += payload_len;
        count++;
    }
}

int bkv_get_key_from_kv(uint8_t* buf, int buf_size, int* is_string_key, char* string_key, int max_string_len, uint64_t* number_key) {
    if (buf_size <= 0) {
        // invalid buf
        return BKV_RESULT_CODE_INVALID_BUF;
    }

    int result_code = 0;
    uint64_t result_length = 0;
    int result_length_bytes_size = 0;
    bkv_decode_length(buf, buf_size, &result_code, &result_length, &result_length_bytes_size);
    if (result_code != 0 || result_length <= 0 || result_length_bytes_size <= 0) {
        // decode length error
        return BKV_RESULT_CODE_FAIL;
    }

    uint8_t* payload = buf + result_length_bytes_size;

    uint8_t first_byte = *payload;
    *is_string_key = 0;
    if ((first_byte & 0x80) != 0) {
        *is_string_key = 1;
    }

    int key_len = first_byte & 0x7F;
    if (key_len > max_string_len) {
        // key len exceeds string len
        return BKV_RESULT_CODE_KEY_LEN_EXCEED_STRING_LEN;
    }
    if (key_len + 1 > buf_size) {
        // key len exceeds buf len
        return BKV_RESULT_CODE_KEY_LEN_EXCEED_BUF_LEN;
    }

    if (*is_string_key == 1) {
        // strcpy
        for (int i = 0; i < key_len; i++) {
            *(string_key + i) = (char) (*(payload + 1 + i));
        }
        *(string_key + key_len) = 0;
    } else {
        *number_key = bkv_decode_number(payload + 1, key_len);
    }

    return BKV_RESULT_CODE_SUCCESS;
}

int bkv_get_value_from_kv(uint8_t* buf, int buf_size, int* pos_begin) {
    if (buf_size <= 0) {
        // invalid buf
        return BKV_RESULT_CODE_INVALID_BUF;
    }

    int result_code = 0;
    uint64_t result_length = 0;
    int result_length_bytes_size = 0;
    bkv_decode_length(buf, buf_size, &result_code, &result_length, &result_length_bytes_size);
    if (result_code != 0 || result_length <= 0 || result_length_bytes_size <= 0) {
        // decode length error
        return BKV_RESULT_CODE_FAIL;
    }

    uint8_t* payload = buf + result_length_bytes_size;

    uint8_t first_byte = *payload;
    int key_len = first_byte & 0x7F;

    if (key_len + 1 > buf_size) {
        // key len exceeds buf len
        return BKV_RESULT_CODE_KEY_LEN_EXCEED_STRING_LEN;
    }

    *pos_begin = result_length_bytes_size + 1 + key_len;

    return BKV_RESULT_CODE_SUCCESS;
}





















