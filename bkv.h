#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if !defined(BKV_H)
#define BKV_H

void dump_buf(char* name, u_int8_t* buf, int buf_size);
void dump_bkv(u_int8_t* buf, int buf_size);

int bkv_append(u_int8_t* buf, int buf_size, int pos, u_int8_t* key, int key_len, int is_string_key, u_int8_t* value, int value_len);
int bkv_append_by_string_key(u_int8_t* buf, int buf_size, int pos, char* key, u_int8_t* value, int value_len);
int bkv_append_by_number_key(u_int8_t* buf, int buf_size, int pos, u_int64_t key, u_int8_t* value, int value_len);

int bkv_get_count(u_int8_t* buf, int buf_size);
int bkv_get_key_by_index(u_int8_t* buf, int buf_size, int index, int* is_string_key, char* string_key, int max_string_len, u_int64_t* number_key);
int bkv_get_value_by_index(u_int8_t* buf, int buf_size, int index, int* pos_begin, int* pos_end);












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