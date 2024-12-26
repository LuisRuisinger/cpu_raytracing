//
// Created by Luis Ruisinger on 02.12.24.
//

#ifndef SOFTWARE_RATYTRACING_TEST_H
#define SOFTWARE_RATYTRACING_TEST_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define LOG_PREFIX(prefix)                                                             \
    do {                                                                              \
        char str_pre[MAX_PREFIX_LEN] = {0};  /* Zero-initialize the buffer */        \
        size_t remaining = MAX_PREFIX_LEN;                                           \
        size_t offset = 0;                                                           \
                                                                                     \
        /* Safe string concatenation with bounds checking */                         \
        int written = snprintf(str_pre + offset, remaining, "%s:", __FILE__);       \
        if (written > 0 && written < remaining) {                                    \
            offset += written;                                                       \
            remaining -= written;                                                    \
        }                                                                           \
                                                                                     \
        /* Add line number */                                                        \
        written = snprintf(str_pre + offset, remaining, "%d:", __LINE__);           \
        if (written > 0 && written < remaining) {                                    \
            offset += written;                                                       \
            remaining -= written;                                                    \
        }                                                                           \
                                                                                     \
        /* Add function name */                                                      \
        written = snprintf(str_pre + offset, remaining, "%s", __FUNCTION__);        \
        if (written > 0 && written < remaining) {                                    \
            offset += written;                                                       \
            remaining -= written;                                                    \
        }                                                                           \
                                                                                     \
        /* Ensure null termination */                                               \
        str_pre[MAX_PREFIX_LEN - 1] = '\0';                                         \
                                                                                     \
        /* Copy to destination if it exists */                                      \
        if (prefix != NULL) {                                                       \
            *prefix = strdup(str_pre);  /* Caller must free this memory */         \
        }                                                                           \
    } while (0)

void log_strings(const char *str, ...) {
    va_list args;
    va_start(args, str);

    const char *it = str;
    while (it != NULL) {
        fprintf(stdout, "%s ", it);
        it = va_arg(args, const char *);
    }

    va_end(args);
}

#define LOG(...) \
    log_strings(__VA_ARGS__, NULL)

#define ASSERT(expr)                                                                              \
    do {                                                                                          \
        if (!(expr)) {                                                                            \
            char **ptr_pre = NULL;                                                                \
                                                                                                  \
            LOG_PREFIX(ptr_pre);                                                                  \
            LOG("ASSERTION FAILED ", ptr_pre);                                                 \
            exit(EXIT_FAILURE);                                                                   \
        }                                                                                         \
    }                                                                                             \
    while (0)

#define TEST(name, fun) \
    void test_##name(void) { \
        fun()                    \
    }

#endif //SOFTWARE_RATYTRACING_TEST_H
