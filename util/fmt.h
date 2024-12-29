//
// Created by Luis Ruisinger on 28.12.24.
//

#ifndef CPU_RAYTRACING_FMT_H
#define CPU_RAYTRACING_FMT_H

#include <string.h>

#include "defines.h"
#include "build.h"

C_GUARD_BEGINN()

typedef enum LogLevel_t {
    NORMAL, DEBUG, ERROR, WARN
} LogLevel;

char *PRINT_FMT(1, 2) fmt(const char *fmt_str, ...);
void  PRINT_FMT(5, 6) fmt_log(
        LogLevel level, const char *file, u32 line, const char *fun, const char *fmt_str, ...);

#define LOG(fmt_str, ...) \
    do { fmt_log(NORMAL, __FILENAME__, __LINE__, __func__, fmt_str, ##__VA_ARGS__); } while (0)

#ifdef DEBUG
    #define DEBUG_LOG(fmt_str, ...) \
        do { fmt_log(DEBUG, __FILENAME__, __LINE__, __func__, fmt_str, ##__VA_ARGS__); } while (0)
#else
    #define DEBUG_LOG(fmt_str, ...)
#endif

void log_handler_init(void);
void log_handler_add_callback(i32 fd, ssize (* fun)(int, const void *, usize));

C_GUARD_END()

#endif //CPU_RAYTRACING_FMT_H
