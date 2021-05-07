

#if !defined(BKV_H)
#define BKV_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define BKV_MAX_STRING_KEY_LEN 32

#define BKV_TRUE 1
#define BKV_FALSE 0

#define BKV_RESULT_CODE_SUCCESS 0
#define BKV_RESULT_CODE_FAIL -1

#define BKV_RESULT_CODE_INVALID_BUF -1
#define BKV_RESULT_CODE_KEY_LEN_EXCEED_STRING_LEN -2
#define BKV_RESULT_CODE_KEY_LEN_EXCEED_BUF_LEN -3
#define BKV_RESULT_CODE_GET_KV_FAIL -4
#define BKV_RESULT_CODE_GET_KEY_FAIL -5
#define BKV_RESULT_CODE_GET_VALUE_FAIL -6
#define BKV_RESULT_CODE_KV_INVALID -7


static const int bkv_endian_i = 1;
#define is_big_endian() ( (*(char*)&bkv_endian_i) == 0 )






typedef int bkv_bool;

void bkv_dump_buf(char* name, uint8_t* buf, int buf_size);
void bkv_dump(uint8_t* buf, int buf_size);

/**
 * encode uint64_t to number buffer to save space
 * @param number
 * @param buf
 * @param pos current buffer pos
 * @return buffer new pos
 */
int bkv_encode_number(uint64_t number, uint8_t* buf, int pos);
uint64_t bkv_decode_number(uint8_t* buf, size_t buf_size);

int bkv_encode_float(float f, uint8_t* buf, int pos);
float bkv_decode_float(uint8_t* buf);

int bkv_append(uint8_t* buf, int buf_size, uint8_t* key, int key_len, int is_string_key, uint8_t* value, int value_len);
int bkv_append_by_string_key(uint8_t* buf, int buf_size, char* key, uint8_t* value, int value_len);
int bkv_append_by_number_key(uint8_t* buf, int buf_size, uint64_t key, uint8_t* value, int value_len);
int bkv_append_number_value_by_string_key(uint8_t* buf, int buf_size, char* key, uint64_t value);
int bkv_append_number_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, uint64_t value);
int bkv_append_string_value_by_string_key(uint8_t* buf, int buf_size, char* key, char* value);
int bkv_append_string_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, char* value);
int bkv_append_float_value_by_string_key(uint8_t* buf, int buf_size, char* key, float value);
int bkv_append_float_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, float value);

bkv_bool bkv_contains_string_key(uint8_t* buf, int buf_size, char* key);
bkv_bool bkv_contains_number_key(uint8_t* buf, int buf_size, uint64_t key);

int bkv_get_value_by_string_key(uint8_t* buf, int buf_size, char* key, int* value_pos_begin, int* value_pos_end);
int bkv_get_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, int* value_pos_begin, int* value_pos_end);
int bkv_get_number_value_by_string_key(uint8_t* buf, int buf_size, char* key, uint64_t* value);
int bkv_get_number_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, uint64_t* value);
int bkv_get_string_value_by_string_key(uint8_t* buf, int buf_size, char* key, char* value);
int bkv_get_string_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, char* value);
int bkv_get_float_value_by_string_key(uint8_t* buf, int buf_size, char* key, float* value);
int bkv_get_float_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, float* value);

int bkv_get_count(uint8_t* buf, int buf_size);
int bkv_get_count_by_key(uint8_t* buf, int buf_size, char* string_key, uint64_t number_key, int is_string_key);

int bkv_get_kv_by_index(uint8_t* buf, int buf_size, int index, int* pos_begin, int* pos_end);
int bkv_get_key_from_kv(uint8_t* buf, int buf_size, int* is_string_key, char* string_key, int max_string_len, uint64_t* number_key);
int bkv_get_value_from_kv(uint8_t* buf, int buf_size, int* pos_begin);

int bkv_get_key_value_by_index(uint8_t* buf, int buf_size, int index, int* is_string_key, char* string_key, int max_string_len, uint64_t* number_key, int* value_pos_begin, int* value_pos_end);
int bkv_get_key_by_index(uint8_t* buf, int buf_size, int index, int* is_string_key, char* string_key, int max_string_len, uint64_t* number_key);
int bkv_get_value_by_index(uint8_t* buf, int buf_size, int index, int* pos_begin, int* pos_end);

int bkv_get_number_value_list_by_key(uint8_t* buf, int buf_size, uint64_t* list, char* string_key, uint64_t number_key, int is_string_key);
int bkv_get_number_value_list_by_string_key(uint8_t* buf, int buf_size, uint64_t* list, char* key);
int bkv_get_number_value_list_by_number_key(uint8_t* buf, int buf_size, uint64_t* list, uint64_t key);












#define USE_TTY 1
#define TIME_FORMAT "%F %T"

#define LOGI(format, ...)                                                    \
    do {                                                                     \
        time_t now = time(NULL);                                             \
        char timestr[20];                                                    \
        strftime(timestr, 20, TIME_FORMAT, localtime(&now));                 \
        if (USE_TTY) {                                                       \
            fprintf(stdout, "\e[01;32m%s INFO: \e[0m" format "\n", timestr, \
                    ## __VA_ARGS__);                                         \
            fflush(stdout);                                                  \
        } else {                                                             \
            fprintf(stdout, "%s INFO: " format "\n", timestr,               \
                    ## __VA_ARGS__);                                         \
            fflush(stdout);                                                  \
        }                                                                    \
    }                                                                        \
    while (0)

#define LOGE(format, ...)                                                     \
    do {                                                                      \
        time_t now = time(NULL);                                              \
        char timestr[20];                                                     \
        strftime(timestr, 20, TIME_FORMAT, localtime(&now));                  \
        if (USE_TTY) {                                                        \
            fprintf(stderr, "\e[01;35m%s ERROR: \e[0m" format "\n", timestr, \
                    ## __VA_ARGS__);                                          \
            fflush(stderr);                                                   \
        } else {                                                              \
            fprintf(stderr, "%s ERROR: " format "\n", timestr,               \
                    ## __VA_ARGS__);                                          \
            fflush(stderr);                                                   \
        }                                                                     \
    }                                                                         \
    while (0)



#endif