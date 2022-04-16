//
// Created by dd on 2022/3/25.
//

#ifndef BKV_BKV_PLUS_H
#define BKV_BKV_PLUS_H

#include "bkv.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void bkv_dump_buf(char* name, const uint8_t* buf, int buf_size);
void bkv_dump(uint8_t* buf, int buf_size);

void bkv_decode_string(const uint8_t* buf, size_t buf_size, char* value);

int bkv_encode_float(float f, uint8_t* buf, int pos);
float bkv_decode_float(uint8_t* buf);

int bkv_encode_double(double f, uint8_t* buf, int pos);
double bkv_decode_double(uint8_t* buf);

int bkv_append_number_value_by_string_key(uint8_t* buf, int buf_size, char* key, uint64_t value);
int bkv_append_number_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, uint64_t value);
int bkv_append_string_value_by_string_key(uint8_t* buf, int buf_size, char* key, char* value);
int bkv_append_string_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, char* value);
int bkv_append_float_value_by_string_key(uint8_t* buf, int buf_size, char* key, float value);
int bkv_append_float_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, float value);
int bkv_append_double_value_by_string_key(uint8_t* buf, int buf_size, char* key, double value);
int bkv_append_double_value_by_number_key(uint8_t* buf, int buf_size, uint64_t key, double value);

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

int bkv_get_count_by_key(uint8_t* buf, int buf_size, char* string_key, uint64_t number_key, int is_string_key);

int bkv_get_key_value_by_index(uint8_t* buf, int buf_size, int index, int* is_string_key, char* string_key, int max_string_len, uint64_t* number_key, int* value_pos_begin, int* value_pos_end);
int bkv_get_key_by_index(uint8_t* buf, int buf_size, int index, int* is_string_key, char* string_key, int max_string_len, uint64_t* number_key);
int bkv_get_value_by_index(uint8_t* buf, int buf_size, int index, int* pos_begin, int* pos_end);

int bkv_get_number_value_list_by_key(uint8_t* buf, int buf_size, uint64_t* list, char* string_key, uint64_t number_key, int is_string_key);
int bkv_get_number_value_list_by_string_key(uint8_t* buf, int buf_size, uint64_t* list, char* key);
int bkv_get_number_value_list_by_number_key(uint8_t* buf, int buf_size, uint64_t* list, uint64_t key);

int bkv_traverse(uint8_t *buf, int buf_size, void (*func)(int is_string_key, char *string_key, uint64_t *number_key, const uint8_t *value, int value_len, void *data), void *func_data);

#define bkv_a(b, bl, k, kl, isk, v, vl) bkv_append(b, bl, k, kl, isk, v, vl)
#define bkv_a_s(b, bl, k, v, vl) bkv_append_by_string_key(b, bl, k, v, vl)
#define bkv_a_s_n(b, l, k, v) bkv_append_number_value_by_string_key(b, l, k, v)
#define bkv_a_s_s(b, l, k, v) bkv_append_string_value_by_string_key(b, l, k, v)
#define bkv_a_s_f(b, l, k, v) bkv_append_float_value_by_string_key(b, l, k, v)

#define bkv_a_n(b, bl, k, v, vl) bkv_append_by_number_key(b, bl, k, v, vl)
#define bkv_a_n_n(b, l, k, v) bkv_append_number_value_by_number_key(b, l, k, v)
#define bkv_a_n_s(b, l, k, v) bkv_append_string_value_by_number_key(b, l, k, v)
#define bkv_a_n_f(b, l, k, v) bkv_append_float_value_by_number_key(b, l, k, v)

#define bkv_c_s(b, l, k) bkv_contains_string_key(b, l, k)
#define bkv_c_n(b, l, k) bkv_contains_number_key(b, l, k)

#define bkv_g_s(b, l, k, vl, ve) bkv_get_value_by_string_key(b, l, k, vl, ve)
#define bkv_g_s_n(b, l, k, v) bkv_get_number_value_by_string_key(b, l, k, v)
#define bkv_g_s_s(b, l, k, v) bkv_get_string_value_by_string_key(b, l, k, v)
#define bkv_g_s_f(b, l, k, v) bkv_get_float_value_by_string_key(b, l, k, v)

#define bkv_g_n(b, l, k, vl, ve) bkv_get_value_by_number_key(b, l, k, vl, ve)
#define bkv_g_n_n(b, l, k, v) bkv_get_number_value_by_number_key(b, l, k, v)
#define bkv_g_n_s(b, l, k, v) bkv_get_string_value_by_number_key(b, l, k, v)
#define bkv_g_n_f(b, l, k, v) bkv_get_float_value_by_number_key(b, l, k, v)

struct bkv_context {
    uint8_t* buf;
    int size;
    int offset;
};








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



#endif //BKV_BKV_PLUS_H
