//
// Created by Luis Ruisinger on 01.12.24.
//

#ifndef SOFTWARE_RATYTRACING_DYNAMIC_ARRAY_H
#define SOFTWARE_RATYTRACING_DYNAMIC_ARRAY_H

#include <stdlib.h>
#include <string.h>

#include "../defines.h"

C_GUARD_BEGINN()

// inspired by the C Code Archive Network - Dynamic Array implementation
#define ARRAY(type) \
    struct { type *mem; usize capacity; usize size; }

typedef ARRAY(i8)            i8_array;
typedef ARRAY(i16)           i16_array;
typedef ARRAY(i32)           i32_array;
typedef ARRAY(i64)           i64_array;

typedef ARRAY(u8)            u8_array;
typedef ARRAY(u16)           u16_array;
typedef ARRAY(u32)           u32_array;
typedef ARRAY(u64)           u64_array;

typedef ARRAY(f32)           f32_array;
typedef ARRAY(f64)           f64_array;

typedef ARRAY(usize)         usize_array;
typedef ARRAY(ssize)         ssize_array;

typedef ARRAY(char)          char_array;
typedef ARRAY(unsigned char) uchar_array;
typedef ARRAY(signed char)   schar_array;

#define ARRAY_TYPEOF(arr) \
    __typeof__(*(arr).mem)

#define ARRAY_ELEMENT(arr, i) \
    (arr).mem[i]

#define ARRAY_CAPACITY_BYTES(arr) \
    (sizeof(*(arr).mem) * (arr).capacity)

#define ARRAY_EMPTY(arr) \
    (!(arr).size)

#ifndef LEAST_ALLOC_SIZE
    // smallest array allocation will be 32
    #define LEAST_ALLOC_SIZE 0x20
#endif

#ifndef ARRAY_ALLOC
    #define ARRAY_ALLOC(arr, n_size)                                                              \
        do {                                                                                      \
            usize align = sizeof(*(arr).mem);                                                     \
            (arr).mem = aligned_alloc(align, align * n_size);                                     \
        }                                                                                         \
        while (0)
#endif

#ifndef ARRAY_FREE
    #define ARRAY_FREE(arr) \
        do { free((arr).mem); } while (0)
#endif

#define ARRAY_MOVE(arr_0, arr_1) \
    do {                         \
        (arr_0).mem = (arr_1).mem; \
        (arr_0).capacity = (arr_1).capacity; \
        (arr_0).size = (arr_1).size;         \
                                 \
        (arr_1).mem = NULL; \
        (arr_1).capacity = 0; \
        (arr_1).size = 0;         \
    }                           \
    while (0)

#ifndef ARRAY_REALLOC_FACTOR
    #define ARRAY_REALLOC_FACTOR 1.0F
#endif

#ifndef ARRAY_REALLOC
    #define ARRAY_REALLOC(arr, n_size)                                                            \
        do {                                                                                      \
            usize align = sizeof(*(arr).mem);                                                     \
            usize arr_size = MIN(n_size, (arr).size);                                             \
                                                                                                  \
            (arr).size = arr_size;                                                                \
            (arr).capacity = n_size;                                                              \
                                                                                                  \
            void *n_ptr = (u8 *) aligned_alloc(align, align * n_size);                            \
            memcpy(n_ptr, (arr).mem, (arr).size);                                                 \
            free((arr).mem);                                                                      \
        }                                                                                         \
        while (0)
#endif

#define ARRAY_NEW() \
    { .mem = NULL, .capacity = 0, .size = 0 }

#define ARRAY_INIT(arr, init)                                                                     \
    do {                                                                                          \
        ARRAY_ALLOC(arr, init);                                                                   \
        (arr).capacity = init;                                                                    \
        (arr).size = 0;                                                                           \
    }                                                                                             \
    while (0)

#define ARRAY_CLEAR(arr) \
    do { (arr).size = 0; } while (0)

#define ARRAY_RESIZE(arr, n_size)                                                                 \
    do {                                                                                          \
        if (n_size != (arr).capacity) {                                                           \
            ARRAY_REALLOC(arr, n_size);                                                           \
        }                                                                                         \
    }                                                                                             \
    while (0)

#define ARRAY_RESERVE(arr, n_size)                                                                \
    do {                                                                                          \
        if (n_size > (arr).capacity) {                                                            \
            ARRAY_REALLOC(arr, n_size);                                                           \
        }                                                                                         \
    }                                                                                             \
    while (0)

#define ARRAY_NEXT_ALLOC(arr, n_capacity)                                                         \
    do {                                                                                          \
        usize n_size = ((arr).capacity | LEAST_ALLOC_SIZE)--;                                     \
        n_size |= n_size >> 1;                                                                    \
        n_size |= n_size >> 2;                                                                    \
        n_size |= n_size >> 4;                                                                    \
        n_size |= n_size >> 8;                                                                    \
        n_size |= n_size >> 16;                                                                   \
        n_size |= n_size >> 32;                                                                   \
        n_size++;                                                                                 \
                                                                                                  \
        *(n_capacity) = n_size;                                                                   \
    }                                                                                             \
    while (0)

#define ARRAY_GROWTH_RESIZE(arr, n_size)                                                          \
    do {                                                                                          \
        if (n_size >= (arr).capacity * ARRAY_REALLOC_FACTOR) {                                    \
            usize n_capacity = (arr).capacity << 1;                                               \
                                                                                                  \
            ARRAY_RESIZE(arr, n_capacity);                                                        \
        }                                                                                         \
    }                                                                                             \
    while (0)

#define ARRAY_APPEND(arr, v)                                                                      \
    do {                                                                                          \
        usize n_elements = sizeof(v) / sizeof(*v);                                                \
        ARRAY_GROWTH_RESIZE(arr, (arr).size + n_elements - 1);                                    \
        memcpy((arr).mem + (arr).size, v, n_elements);                                            \
        (arr).size += n_elements;                                                                 \
    }                                                                                             \
    while (0)

#define ARRAY_PREPEND(arr, v)                                                                     \
    do {                                                                                          \
        usize n_elements = sizeof(v) / sizeof(*v);                                                \
        ARRAY_GROWTH_RESIZE(arr, (arr).size + n_elements - 1);                                    \
        memmove((arr).mem + n_elements, (arr).mem, (arr).size);                                   \
        memcpy((arr).mem, v, n_elements);                                                         \
        (arr).size += n_elements;                                                                 \
    }                                                                                             \
    while (0)

#define ARRAY_PUSH_BACK(arr, v)                                                                   \
    do {                                                                                          \
        ARRAY_GROWTH_RESIZE((arr), (arr).size);                                                   \
        ARRAY_ELEMENT((arr), (arr).size) = v;                                                     \
        ++(arr).size;                                                                             \
    }                                                                                             \
    while (0)

#define ARRAY_PUSH_BACK_MULTIPLE(arr, ...)                                                        \
    do {                                                                                          \
        __typeof__(*(arr).mem) tmp_arr[] = { __VA_ARGS__ };                                       \
        ARRAY_APPEND(arr, tmp_arr);                                                               \
    }                                                                                             \
    while (0)

#define ARRAY_PUSH_FRONT(arr, v)                                                                  \
    do {                                                                                          \
        ARRAY_GROWTH_RESIZE(arr, (arr).size);                                                     \
        memmove((arr).mem + 1, (arr).mem, (arr).size);                                            \
        ARRAY_ELEMENT(arr, 0) = v;                                                                \
        ++(arr).size;                                                                             \
    }                                                                                             \
    while (0)

#define ARRAY_PUSH_FRONT_MULTIPLE(arr, ...)                                                       \
    do {                                                                                          \
        __typeof__(*(arr).mem) tmp_arr[] = { __VA_ARGS__ };                                       \
        ARRAY_PREPEND(arr, tmp_arr);                                                              \
    }                                                                                             \
    while (0)

#define ARRAY_REMOVE(arr, i)                                                                      \
    do {                                                                                          \
        if ((i) < (arr).size - 1) {                                                               \
            memmove(                                                                              \
                &ARRAY_ELEMENT(arr, (i)), &ARRAY_ELEMENT(arr, (i) + 1),                           \
                ((arr).size - 1 - (i)) * sizeof(*(arr).mem));                                     \
        }                                                                                         \
                                                                                                  \
        --(arr).size;                                                                             \
    }                                                                                             \
    while (0)

#define ARRAY_INSERT(arr, i, v) \
    do { ARRAY_ELEMENT(arr, i) = v; } while (0)

#define ARRAY_FOREACH_SUBRANGE(arr, i, a, b) \
    for ((i) = &(arr).mem[(a)]; (i) < &(arr).mem[(b)]; ++(i))

#define ARRAY_FOREACH_REVERSE_SUBRANGE(arr, i, a, b) \
    for ((i) = &(arr).mem[(a)]; (i) >= &(arr).mem[(b) + 1]; --(i))

#define ARRAY_FOREACH(arr, i) \
    ARRAY_FOREACH_SUBRANGE(arr, i, 0, (arr).size)

#define ARRAY_FOREACH_REVERSE(arr, i) \
    ARRAY_FOREACH_REVERSE_SUBRANGE(arr, i, (arr).size - 1, -1)

#define ARRAY_SORT(arr, cmp) \
    qsort((arr).mem, (arr).size, sizeof(ARRAY_TYPEOF(arr)), cmp)

C_GUARD_END()

#endif //SOFTWARE_RATYTRACING_DYNAMIC_ARRAY_H

