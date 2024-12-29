//
// Created by Luis Ruisinger on 28.12.24.
//

#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "fmt.h"
#include "../util/datastructures/dynamic_array.h"

static char *v_fmt(const char *fmt_str, va_list args) {
    char *ptr;
    if (vasprintf(&ptr, fmt_str, args) == -1) {
        ptr = NULL;
    }

    return ptr;
}

char *PRINT_FMT(1, 2) fmt(const char *fmt_str, ...) {
    va_list args;
    va_start(args, fmt_str);

    char *ptr = v_fmt(fmt_str, args);

    va_end(args);
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

typedef struct LogCallback_t {
    i32 fd;
    ssize (* fun)(int, const void *, usize);
} LogCallback;

typedef struct LogHandler_t {
    ARRAY(LogCallback) callbacks;
    pthread_mutex_t mutex;
} LogHandler;

LogHandler log_handler;

void log_handler_init(void) {
    pthread_mutex_init(&log_handler.mutex, NULL);

    ARRAY_INIT(log_handler.callbacks, 8);
    log_handler_add_callback(STDOUT_FILENO, write);
}

void log_handler_add_callback(i32 fd, ssize (* fun)(int, const void *, usize)) {
    LogCallback callback = { fd, fun };
    ARRAY_PUSH_BACK(log_handler.callbacks, callback);
}

void PRINT_FMT(5, 6) fmt_log(
        LogLevel level, const char *file, u32 line, const char *fun, const char *fmt_str, ...) {
    va_list args;
    va_start(args, fmt_str);

    char *ptr = v_fmt(fmt_str, args);
    va_end(args);

    if (!ptr) {
        return;
    }

    char prefix[1024] = { 0 };
    switch (level) {
        case NORMAL:
            snprintf(prefix, sizeof(prefix),
                     "[\033[38;5;46mLOG\033[0m][%s:%d][%s] ", file, line, fun);
            break;
        case DEBUG:
            snprintf(prefix, sizeof(prefix),
                     "[\033[38;5;208mDEBUG\033[0m][%s:%d][%s] ", file, line, fun);
            break;
        case ERROR:
            snprintf(prefix, sizeof(prefix),
                     "[\033[1;31mERROR\033[0m][%s:%d][%s] ", file, line, fun);
            break;
        case WARN:
            snprintf(prefix, sizeof(prefix),
                     "[\033[38;5;226mWARN\033[0m[%s:%d][%s] ", file, line, fun);
            break;
    }

    usize app = strlen(ptr);
    usize pre = strlen(prefix);

    char *concat = realloc(ptr, app + pre + 2);
    if (!concat) {
        free(ptr);
        return;
    }

    ptr = concat;
    ptr[app + pre + 0] = '\n';
    ptr[app + pre + 1] = '\0';

    memmove(ptr + pre, ptr, app);
    memcpy(ptr, prefix, pre);

    {
        pthread_mutex_lock(&log_handler.mutex);

        LogCallback *callback = NULL;
        ARRAY_FOREACH(log_handler.callbacks, callback) {
            callback->fun(callback->fd, ptr, app + pre + 2);
        }

        pthread_mutex_unlock(&log_handler.mutex);
    }
    free(ptr);
}
