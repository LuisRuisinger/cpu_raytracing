//
// Created by Luis Ruisinger on 06.01.25.
//

#ifndef CPU_RAYTRACING_VEC2_H
#define CPU_RAYTRACING_VEC2_H

#include "../defines.h"

C_GUARD_BEGINN()

typedef struct vec2f_t {
    f32 x;
    f32 y;
} __attribute__((aligned(8))) vec2f;

#define VEC2_1(_x) \
    (vec2f) { .x = _x, .y = _x }

#define VEC2_2(_x, _y) \
    (vec2f) { .x = _x, .y = _y }

#define VEC2_GET_MACRO(_1, _2, NAME, ...) NAME

#define VEC2(...) \
    VEC2_GET_MACRO(__VA_ARGS__, VEC2_2, VEC2_1, INVALID)(__VA_ARGS__)

#define GET_VEC2_X(_v) \
    (_v).x

#define GET_VEC2_Y(_v) \
    (_v).y

#define SET_VEC2_X(_v, _x) \
    ((_v).x = _x)

#define SET_VEC2_Y(_v, _y) \
    ((_v).y = _y)

#define VEC2_SIGN(_v) \
    (((GET_VEC2_X(_v) < 0) || (GET_VEC2_Y(_v) < 0)) ? -1 : +1)

ALWAYS_INLINE static inline vec2f vec2_add(vec2f a, vec2f b) {
    return (vec2f) {
            .x = a.x + b.x,
            .y = a.y + b.y
    };
}

ALWAYS_INLINE static inline vec2f vec2_adds(vec2f a, f32 scalar) {
    return (vec2f) {
            .x = a.x + scalar,
            .y = a.y + scalar
    };
}

ALWAYS_INLINE static inline vec2f vec2_sub(vec2f a, vec2f b) {
    return (vec2f) {
            .x = a.x - b.x,
            .y = a.y - b.y
    };
}

ALWAYS_INLINE static inline vec2f vec2_subs(vec2f a, f32 scalar) {
    return (vec2f) {
            .x = a.x - scalar,
            .y = a.y - scalar
    };
}

ALWAYS_INLINE static inline vec2f vec2_mul(vec2f a, vec2f b) {
    return (vec2f) {
            .x = a.x * b.x,
            .y = a.y * b.y
    };
}

ALWAYS_INLINE static inline vec2f vec2_muls(vec2f a, f32 scalar) {
    return (vec2f) {
            .x = a.x * scalar,
            .y = a.y * scalar
    };
}

ALWAYS_INLINE static inline vec2f vec2_divs(vec2f a, f32 scalar) {
    return (vec2f) {
            .x = a.x / scalar,
            .y = a.y / scalar
    };
}

ALWAYS_INLINE static inline f32 vec2_length(vec2f a) {
    return sqrtf(a.x * a.x + a.y * a.y);
}

ALWAYS_INLINE static inline vec2f vec2_normalize(vec2f a) {
    return vec2_muls(a, vec2_length(a));
}

ALWAYS_INLINE static inline f32 vec2_dot(vec2f a, vec2f b) {
    return a.x * b.x + a.y + b.y;
}

ALWAYS_INLINE static inline vec2f vec2_inverse(vec2f a) {
    return (vec2f) {
            a.x != 0.0F ? 1.0F / a.x : INFINITY,
            a.y != 0.0F ? 1.0F / a.y : INFINITY,
    };
}

ALWAYS_INLINE static inline vec2f vec2_min(vec2f a, vec2f b) {
    return (vec2f) {
            .x = a.x < b.x ? a.x : b.x,
                         .y = a.y < b.y ? a.y : b.y,
    };
}

ALWAYS_INLINE static inline vec2f vec2_max(vec2f a, vec2f b) {
    return (vec2f) {
            .x = a.x > b.x ? a.x : b.x,
            .y = a.y > b.y ? a.y : b.y,
    };
}

ALWAYS_INLINE static inline vec2f vec2_refract(vec2f i, vec2f n, f32 eta) {
    f32 tmp_0 = vec2_dot(n, i);
    f32 k = 1.0F - eta * eta * (1.0F - tmp_0 * tmp_0);

    if (k < 0.0F) {
        return VEC2(0.0F);
    }

    f32 tmp_1 = eta * tmp_0 + sqrtf(k);
    n = vec2_muls(n, tmp_1);
    i = vec2_muls(i, eta);

    return vec2_sub(i, n);
}

ALWAYS_INLINE static inline vec2f rec2_reflect(vec2f i, vec2f n) {
    f32 tmp_0 = 2.0F * vec2_dot(n, i);
    n = vec2_muls(n, tmp_0);

    return vec2_sub(i, n);
}

C_GUARD_END()

#endif //CPU_RAYTRACING_VEC2_H
