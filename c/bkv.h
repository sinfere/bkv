

#if !defined(BKV_H)
#define BKV_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define BKV_MAX_STRING_KEY_LEN 32

#define BKV_TRUE 1
#define BKV_FALSE 0

#define BKV_RESULT_CODE_SUCCESS 0
#define BKV_RESULT_CODE_FAIL (-1)

#define BKV_RESULT_CODE_INVALID_BUF (-1)
#define BKV_RESULT_CODE_KEY_LEN_EXCEED_STRING_LEN (-2)
#define BKV_RESULT_CODE_KEY_LEN_EXCEED_BUF_LEN (-3)
#define BKV_RESULT_CODE_GET_KV_FAIL (-4)
#define BKV_RESULT_CODE_GET_KEY_FAIL (-5)
#define BKV_RESULT_CODE_GET_VALUE_FAIL (-6)
#define BKV_RESULT_CODE_KV_INVALID (-7)


static const int bkv_endian_i = 1;
#define is_big_endian() ( (*(char*)&bkv_endian_i) == 0 )


typedef int bkv_bool;

void bkv_reverse(uint8_t* bs, size_t size);

/**
 * encode uint64_t to number buffer to save space
 * @param number
 * @param buf
 * @param pos current buffer pos
 * @return buffer new pos
 */
int bkv_encode_number(uint64_t number, uint8_t* buf, int pos);
uint64_t bkv_decode_number(const uint8_t* buf, size_t buf_size);
void bkv_decode_length(const uint8_t* buf, size_t buf_size, int* result_code, uint64_t* result_length, int* result_length_byte_size);


int bkv_append(uint8_t* buf, int buf_size, const uint8_t* key, int key_len, int is_string_key, const uint8_t* value, int value_len);
int bkv_append_by_string_key(uint8_t* buf, int buf_size, char* key, uint8_t* value, int value_len);
int bkv_append_by_number_key(uint8_t* buf, int buf_size, uint64_t key, uint8_t* value, int value_len);

bkv_bool bkv_contains_key(uint8_t* buf, int buf_size, char* string_key, uint64_t number_key, int is_string_key);

int bkv_get_count(uint8_t* buf, int buf_size);


int bkv_get_kv_by_index(uint8_t* buf, int buf_size, int index, int* pos_begin, int* pos_end);
int bkv_get_key_from_kv(uint8_t* buf, int buf_size, int* is_string_key, char* string_key, int max_string_len, uint64_t* number_key);
int bkv_get_value_from_kv(uint8_t* buf, int buf_size, int* pos_begin);


#endif