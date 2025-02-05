//
// Created by Luis Ruisinger on 01.12.24.
//

#ifndef SOFTWARE_RATYTRACING_DYNAMIC_ARRAY_H
#define SOFTWARE_RATYTRACING_DYNAMIC_ARRAY_H

#include <stdlib.h>
#include <string.h>

#include "../defines.h"
#include "../fmt.h"

C_GUARD_BEGINN()

// inspired by the C Code Archive Network - Dynamic Array implementation
#define ARRAY(_type) \
    struct { _type *mem; usize capacity; usize size; }

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

#define ARRAY_TYPEOF(_arr) \
    __typeof__(*(_arr).mem)

#define ARRAY_ELEMENT(_arr, _i) \
    (_arr).mem[_i]

#define ARRAY_CAPACITY_BYTES(_arr) \
    (sizeof(*(_arr).mem) * (_arr).capacity)

#define ARRAY_EMPTY(_arr) \
    (!(_arr).size)

#ifndef LEAST_ALLOC_SIZE
    // smallest array allocation will be 32
    #define LEAST_ALLOC_SIZE 0x20
#endif

// if 0 then LEAST_ALLOC
#define ARRAY_NEXT_ALLOC(_cap)                                                                    \
    do {                                                                                          \
        usize __cap = MAX(*(_cap) - 1, LEAST_ALLOC_SIZE - 1);                                     \
        __cap |= __cap >> 1;                                                                      \
        __cap |= __cap >> 2;                                                                      \
        __cap |= __cap >> 4;                                                                      \
        __cap |= __cap >> 8;                                                                      \
        __cap |= __cap >> 16;                                                                     \
        __cap |= __cap >> 32;                                                                     \
        __cap++;                                                                                  \
                                                                                                  \
        *(_cap) = __cap;                                                                          \
    }                                                                                             \
    while (0)

#ifndef ARRAY_ALLOC
    #define ARRAY_ALLOC(_arr, _cap) \
        do { (_arr).mem = malloc(sizeof(ARRAY_TYPEOF(_arr)) * (_cap)); } while (0)
#endif

#ifndef ARRAY_FREE
    #define ARRAY_FREE(_arr) \
        do { free((_arr).mem); } while (0)
#endif

#define ARRAY_MOVE(_dst, _src) \
    do {                         \
        (_dst).mem = (_src).mem; \
        (_dst).capacity = (_src).capacity; \
        (_dst).size = (_src).size;         \
                                 \
        (_src).mem = NULL; \
        (_src).capacity = 0; \
        (_src).size = 0;         \
    }                           \
    while (0)

#define ARRAY_SWAP(_a, _b)      \
    do {                        \  
        __typeof__(_a) _c = _a; \
        _a = _b;                \
        _b = _c;                \
    } while (0)                 

#ifndef ARRAY_REALLOC_FACTOR

    // factor on which the underlying continous space is considered full
    // this should never be greater than 1.0 or near to/smaller than 0
    #define ARRAY_REALLOC_FACTOR 1.0F
#endif

#ifndef ARRAY_REALLOC
    #define ARRAY_REALLOC(_arr, _cap)                                                             \
        do {                                                                                      \
                                                                                                  \
            (_arr).size = MIN(_cap, (_arr).size);                                                 \
            (_arr).capacity = _cap;                                                               \
                                                                                                  \
            void *_ptr = (u8 *) malloc(sizeof(ARRAY_TYPEOF(_arr)) * (_cap));                      \
            memcpy(_ptr, (_arr).mem, (_arr).size);                                                \
            free((_arr).mem);                                                                     \
            (_arr).mem = _ptr;                                                                    \
        }                                                                                         \
        while (0)
#endif

#define ARRAY_NEW() \
    { .mem = NULL, .capacity = 0, .size = 0 }

#define ARRAY_INIT(_arr, _init)                                                                   \
    do {                                                                                          \
        usize __init = _init;                                                                     \
        ARRAY_NEXT_ALLOC(&__init);                                                                \
                                                                                                  \
        ARRAY_ALLOC(_arr, __init);                                                                \
        (_arr).capacity = __init;                                                                 \
        (_arr).size = 0;                                                                          \
    }                                                                                             \
    while (0)

#define ARRAY_INIT_EXPLICIT(_arr, _init)                                                          \
    do {                                                                                          \
        usize __init = _init;                                                                     \
        ARRAY_ALLOC(_arr, __init);                                                                \
                                                                                                  \
        (_arr).capacity = __init;                                                                 \
        (_arr).size = 0;                                                                          \
    }                                                                                             \
    while (0)

#define ARRAY_CLEAR(_arr) \
    do { (_arr).size = 0; } while (0)

#define ARRAY_RESIZE(_arr, _cap)                                                                  \
    do {                                                                                          \
        if ((_cap) != (_arr).capacity) {                                                          \
            ARRAY_REALLOC(_arr, _cap);                                                            \
        }                                                                                         \
    }                                                                                             \
    while (0)

#define ARRAY_RESERVE(_arr, _cap)                                                                 \
    do {                                                                                          \
        if ((_cap) > (_arr).capacity) {                                                           \
            ARRAY_REALLOC(_arr, _cap);                                                            \
        }                                                                                         \
    }                                                                                             \
    while (0)

#define ARRAY_GROWTH_RESIZE(_arr, _cap)                                                           \
    do {                                                                                          \
        if (_cap >= (_arr).capacity * ARRAY_REALLOC_FACTOR) {                                     \
            ARRAY_RESIZE(_arr, (_arr).capacity << 1);                                             \
        }                                                                                         \
    }                                                                                             \
    while (0)

#define ARRAY_APPEND(_arr, _v)                                                                    \
    do {                                                                                          \
        usize _size = sizeof(_v) / sizeof(*(_v));                                                 \
        ARRAY_GROWTH_RESIZE(_arr, (_arr).size + _size - 1);                                       \
        memcpy((_arr).mem + (_arr).size, _v, sizeof(_v));                                         \
        (_arr).size += _size;                                                                     \
    }                                                                                             \
    while (0)

#define ARRAY_PREPEND(_arr, _v)                                                                   \
    do {                                                                                          \
        usize _size = sizeof(_v) / sizeof(*(_v));                                                 \
        ARRAY_GROWTH_RESIZE(_arr, (_arr).size + _size - 1);                                       \
        memmove((_arr).mem + _size, (_arr).mem, (_arr).size);                                     \
        memcpy((_arr).mem, _v, _size);                                                            \
        (_arr).size += _size;                                                                     \
    }                                                                                             \
    while (0)

#define ARRAY_PUSH_BACK(_arr, _v)                                                                 \
    do {                                                                                          \
        ARRAY_GROWTH_RESIZE((_arr), (_arr).size);                                                 \
        ARRAY_ELEMENT((_arr), (_arr).size) = _v;                                                  \
        ++(_arr).size;                                                                            \
    }                                                                                             \
    while (0)

#define ARRAY_PUSH_BACK_MULTIPLE(_arr, ...)                                                       \
    do {                                                                                          \
        __typeof__(*(_arr).mem) __arr[] = { __VA_ARGS__ };                                        \
        ARRAY_APPEND(_arr, __arr);                                                                \
    }                                                                                             \
    while (0)

#define ARRAY_PUSH_FRONT(_arr, _v)                                                                \
    do {                                                                                          \
        ARRAY_GROWTH_RESIZE(_arr, (_arr).size);                                                   \
        memmove((_arr).mem + 1, (_arr).mem, (_arr).size);                                         \
        ARRAY_ELEMENT(_arr, 0) = _v;                                                              \
        ++(_arr).size;                                                                            \
    }                                                                                             \
    while (0)

#define ARRAY_PUSH_FRONT_MULTIPLE(_arr, ...)                                                      \
    do {                                                                                          \
        __typeof__(*(_arr).mem) __arr[] = { __VA_ARGS__ };                                        \
        ARRAY_PREPEND(_arr, __arr);                                                               \
    }                                                                                             \
    while (0)

#define ARRAY_REMOVE(_arr, _i)                                                                    \
    do {                                                                                          \
        if ((_i) < (_arr).size - 1) {                                                             \
            memmove(                                                                              \
                &ARRAY_ELEMENT(_arr, (_i)), &ARRAY_ELEMENT(_arr, (_i) + 1),                       \
                ((_arr).size - 1 - (_i)) * sizeof(*(_arr).mem));                                  \
        }                                                                                         \
                                                                                                  \
        --(_arr).size;                                                                            \
    }                                                                                             \
    while (0)

#define ARRAY_INSERT(_arr, _i, _v) \
    do { ARRAY_ELEMENT(_arr, _i) = _v; } while (0)

#define ARRAY_FOREACH_SUBRANGE(_arr, _i, _a, _b) \
    for ((_i) = &(_arr).mem[(_a)]; (_i) < &(_arr).mem[(_b)]; ++(_i))

#define ARRAY_FOREACH_REVERSE_SUBRANGE(_arr, _i, _a, _b) \
    for ((_i) = &(_arr).mem[(_a)]; (_i) >= &(_arr).mem[(_b) + 1]; --(_i))

#define ARRAY_FOREACH(_arr, _i) \
    ARRAY_FOREACH_SUBRANGE(_arr, _i, 0, (_arr).size)

#define ARRAY_FOREACH_REVERSE(_arr, _i) \
    ARRAY_FOREACH_REVERSE_SUBRANGE(_arr, _i, (_arr).size - 1, -1)

#define ARRAY_SORT(_arr, _fun) \
    qsort((_arr).mem, (_arr).size, sizeof(ARRAY_TYPEOF(_arr)), _fun)

C_GUARD_END()

#endif //SOFTWARE_RATYTRACING_DYNAMIC_ARRAY_H

