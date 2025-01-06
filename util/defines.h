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

#define DIMENSIONS 3

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

static ALWAYS_INLINE f32 get128_reload(__m128 a, i32 i) {
    alignas(16) f32 tmp[8];
    _mm_store_ps(tmp, a);
    return tmp[i];
}

C_GUARD_END()

#endif //SOFTWARE_RATYTRACING_DEFINES_H
