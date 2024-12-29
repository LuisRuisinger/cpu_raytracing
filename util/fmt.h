//
// Created by Luis Ruisinger on 28.12.24.
//

#ifndef CPU_RAYTRACING_FMT_H
#define CPU_RAYTRACING_FMT_H

#include <string.h>

#include "defines.h"
#include "build.h"

C_GUARD_BEGINN()

char *PRINT_FMT(1, 2) fmt(const char *fmt_str, ...);
void  PRINT_FMT(4, 5) fmt_log(const char *file, u32 line, const char *fun, const char *fmt_str, ...);

#define LOG(fmt_str, ...) \
    do { fmt_log(__FILE__, __LINE__, __func__, fmt_str, ##__VA_ARGS__); } while (0)

#ifdef DEBUG
    #define DEBUG_LOG(fmt_str, ...) \
        LOG(fmt_str, ##__VA_ARGS__)
#else
    #define DEBUG_LOG(fmt_str, ...)
#endif

C_GUARD_END()

#endif //CPU_RAYTRACING_FMT_H
