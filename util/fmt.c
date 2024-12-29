//
// Created by Luis Ruisinger on 28.12.24.
//

#include <stdarg.h>
#include <stdio.h>

#include "fmt.h"

char *PRINT_FMT(1, 2) fmt(const char *fmt_str, ...) {
    va_list list;
    char *ptr;

    va_start(list, fmt_str);
    if (vasprintf(&ptr, fmt_str, list) == -1) {
        ptr = NULL;
    }

    va_end(list);
    return ptr;
}

// vasprintf implementation if not available
#if !defined(__GNUC__)
#include <stdarg.h>
#include <stdlib.h>

static i32 vasprintf(char **res, const char *fmt_str, va_list args) {
    va_list args_cpy;
    va_copy(args_cpy, args);

    i32 len = vsnprintf(NULL, 0, fmt_str, args_cpy);
    va_end(args_cpy);

    if (len < 0 || !(*res = malloc(len + 1))) {
        return -1;
    }

    return vsprintf(*res, fmt_str, args);
}

#endif

void PRINT_FMT(4, 5) fmt_log(
        const char *file, u32 line, const char *fun, const char *fmt_str, ...) {
    va_list args;
    va_start(args, fmt_str);

    char *ptr = fmt(fmt_str, args);
    if (ptr) {
        char prefix[1024] = { 0 };
        snprintf(prefix, sizeof(prefix), "[%s:%d][%s] ", file, line, fun);

        usize app = strlen(ptr);
        usize pre = strlen(prefix);

        char *concat = realloc(ptr, app + pre + 1);
        if (!concat) {
            free(ptr);
            return;
        }

        ptr = concat;
        memmove(ptr + pre, ptr, app + 1);
        memcpy(ptr, prefix, pre);

        fprintf(stdout, "%s\n", ptr);
        free(ptr);
    }
}
