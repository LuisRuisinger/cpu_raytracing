//
// Created by Luis Ruisinger on 24.11.24.
//

#ifndef SOFTWARE_RAYTRACING_VEC3_H
#define SOFTWARE_RAYTRACING_VEC3_H

#include <math.h>
#include <stdatomic.h>

#include "../defines.h"
#include "vec4.h"

C_GUARD_BEGINN()

typedef vec4f vec3f;

#define VEC3_PAD 0.0F

#define VEC3_1(_x) \
    VEC4_4(_x, _x, _x, VEC3_PAD)

#define VEC3_2(_x, _y) \
    VEC4_4(_x, _y, _y, VEC3_PAD)

#define VEC3_3(_x, _y, _z) \
    VEC4_4(_x, _y, _z, VEC3_PAD)

#define VEC3_GET_MACRO(_1, _2, _3, NAME, ...) NAME

#define VEC3(...) \
    VEC3_GET_MACRO(__VA_ARGS__, VEC3_3, VEC3_2, VEC3_1, INVALID)(__VA_ARGS__)

#define VEC3_SIGN(_v) \
    VEC4_SIGN(_v)

#define VEC3_GET(_v, _i) \
    VEC4_GET(_v, _i)

#define VEC3_SET(_v, _i, _val) \
    VEC4_SET(_v, _i, _val)

ALWAYS_INLINE static inline f32 vec3_hsum(vec3f v) {
    return _mm_hsum_ps(v.vec);
}

ALWAYS_INLINE static inline vec3f vec3_add(vec3f a, vec3f b) {
    return vec4_add(a, b);
}

ALWAYS_INLINE static inline vec3f vec3_adds(vec3f a, f32 scalar) {
    a = vec4_adds(a, scalar);
    VEC4_SET(a, 3, 0.0F);

    return a;
}

ALWAYS_INLINE static inline vec3f vec3_sub(vec3f a, vec3f b) {
    return vec4_sub(a, b);
}

ALWAYS_INLINE static inline vec3f vec3_subs(vec3f a, f32 scalar) {
    a = vec4_subs(a, scalar);
    VEC4_SET(a, 3, 0.0F);

    return a;
}

ALWAYS_INLINE static inline vec3f vec3_mul(vec3f a, vec3f b) {
    return vec4_mul(a, b);
}

ALWAYS_INLINE static inline vec3f vec3_muls(vec3f a, f32 scalar) {
    return vec4_muls(a, scalar);
}

ALWAYS_INLINE static inline vec3f vec3_div(vec3f a, vec3f b) {
    a = vec4_div(a, b);
    VEC4_SET(a, 3, 0.0F);

    return a;
}

ALWAYS_INLINE static inline vec3f vec3_divs(vec3f a, f32 scalar) {
    a = vec4_divs(a, scalar);
    VEC4_SET(a, 3, 0.0F);

    return a;
}

ALWAYS_INLINE static inline f32 vec3_length(vec3f a) {
    return vec4_length(a);
}

ALWAYS_INLINE static inline vec3f vec3_normalize(vec3f a) {
    return vec4_normalize(a);
}

ALWAYS_INLINE static inline f32 vec3_dot(vec3f a, vec3f b) {
    return vec4_dot(a, b);
}

ALWAYS_INLINE static inline vec3f vec3_cross(vec3f a, vec3f b) {
    __m128 _tmp_0 = _mm_shuffle_ps(a.vec, a.vec, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 _tmp_1 = _mm_shuffle_ps(b.vec, b.vec, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 _tmp_2 = _mm_mul_ps(_tmp_0, b.vec);
    __m128 _tmp_3 = _mm_mul_ps(_tmp_0, _tmp_1);
    __m128 _tmp_4 = _mm_shuffle_ps(_tmp_2, _tmp_2, _MM_SHUFFLE(3, 0, 2, 1));

    return (vec3f) { .vec = _mm_sub_ps(_tmp_3, _tmp_4) };
}

ALWAYS_INLINE static inline vec3f vec3_inverse(vec3f a) {
    a = vec4_inverse(a);
    VEC4_SET(a, 3, 0.0F);

    return a;
}

ALWAYS_INLINE static inline vec3f vec3_min(vec3f a, vec3f b) {
    return vec4_min(a, b);
}

ALWAYS_INLINE static inline vec3f vec3_max(vec3f a, vec3f b) {
    return vec4_max(a, b);
}

ALWAYS_INLINE static inline vec3f vec3_refract(vec3f i, vec3f n, f32 eta) {
    f32 tmp_0 = vec3_dot(n, i);
    f32 k = 1.0F - eta * eta * (1.0F - tmp_0 * tmp_0);

    if (k < 0.0F) {
        return VEC3(0.0F);
    }

    f32 tmp_1 = eta * tmp_0 + sqrtf(k);
    n = vec3_muls(n, tmp_1);
    i = vec3_muls(i, eta);

    return vec3_sub(i, n);
}

ALWAYS_INLINE static inline vec3f vec3_reflect(vec3f i, vec3f n) {
    f32 tmp_0 = 2.0F * vec3_dot(n, i);
    n = vec3_muls(n, tmp_0);

    return vec3_sub(i, n);
}

C_GUARD_END()

#endif //SOFTWARE_RAYTRACING_VEC3_H
