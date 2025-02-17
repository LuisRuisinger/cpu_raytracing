//
// Created by Luis Ruisinger on 24.11.24.
//

#ifndef SOFTWARE_RATYTRACING_DEFINES_H
#define SOFTWARE_RATYTRACING_DEFINES_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdalign.h>

#include "build.h"
#include "builtin.h"

C_GUARD_BEGINN()

#define BYTE_SIZE 8
#define DIMENSIONS 3
#define DEFAULT_WIDTH  680
#define DEFAULT_HEIGHT 480

#define MAX(x, y) \
    (((x) > (y)) ? (x) : (y))

#define MIN(x, y) \
    (((x) < (y)) ? (x) : (y))

#define TO_RADIANS(deg) \
    ((deg) * M_PI / 180.0F)

#define TO_DEGREES(rad) \
    ((rad) * 180.0F / M_PI)

#define FAST_MOD_POW2(num, mod) \
    ((num) & (mod - 1))

#define RND_POW_2(_n, _p) \
    (((_n) + (_p) - 1) & ~((_p) - 1))

#define BITWISE_U32(_type, _t) \
    ((union { u32 u; _type t; }) { .t = _t }.u)

#define BITWISE_TYPE(_type, _u) \
    ((union { u32 u; _type t; }) { .u = _u }.t)

// IEEE 754
#define MANTISSA_LEN 23
#define EXPONENT_LEN 8
#define COMPARATOR_F32(_a, _b, _r)                                                                \
    do {                                                                                          \
        union {                                                                                   \
            f32 f; u32 u;                                                                         \
        } cast = { .f = (_a) - (_b) };                                                            \
                                                                                                  \
        *(_r) =                                                                                   \
            -!!(cast.u & (1 << (MANTISSA_LEN + EXPONENT_LEN))) |                                  \
            !!((u8) (cast.u >> MANTISSA_LEN));                                                    \
    }                                                                                             \
    while (0)

#define IS_FLOAT_0(_f, _r)                                                                        \
    do {                                                                                          \
        union {                                                                                   \
            f32 f; u32 u;                                                                         \
        } cast = { .f = (_r) };                                                                   \
                                                                                                  \
        *(_r) = cast.u & ((1 << 31) - 1)                                                          \
    }                                                                                             \
    while (0)

C_GUARD_END()

#endif //SOFTWARE_RATYTRACING_DEFINES_H
