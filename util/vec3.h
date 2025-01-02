//
// Created by Luis Ruisinger on 24.11.24.
//

#ifndef SOFTWARE_RAYTRACING_VEC3_H
#define SOFTWARE_RAYTRACING_VEC3_H

#include <math.h>
#include <stdatomic.h>

#include "defines.h"

C_GUARD_BEGINN()

typedef struct vec3f_t {
    __m128 vec;
} __attribute__((aligned(16))) vec3f;

typedef struct vec2f_t {
    f32 x;
    f32 y;
} __attribute__((aligned(8))) vec2f;

#define VEC3_PAD 0.0F

#define VEC3_1(x) \
    (vec3f) { .vec = _mm_set_ps(VEC3_PAD, x, x, x) }

#define VEC3_2(x, y) \
    (vec3f) { .vec = _mm_set_ps(VEC3_PAD, y, y, x) }

#define VEC3_3(x, y, z) \
    (vec3f) { .vec = _mm_set_ps(VEC3_PAD, z, y, x) }

#define VEC3_GET_MACRO(_1, _2, _3, NAME, ...) NAME

#define VEC3(...) \
    VEC3_GET_MACRO(__VA_ARGS__, VEC3_3, VEC3_2, VEC3_1)(__VA_ARGS__)

#define GET_VEC_X(v) \
    _mm_cvtss_f32(v.vec)

#define GET_VEC_Y(v) \
    _mm_cvtss_f32(_mm_shuffle_ps((v).vec, (v).vec, _MM_SHUFFLE(0, 0, 0, 1)))

#define GET_VEC_Z(v) \
    _mm_cvtss_f32(_mm_shuffle_ps((v).vec, (v).vec, _MM_SHUFFLE(0, 0, 0, 2)))

#define SET_VEC_X(v, x)                                                                           \
    do {                                                                                          \
        __m128 tmp_0 = _mm_set1_ps(x);                                                            \
        (v)->vec = _mm_blend_ps((v)->vec, tmp_0, 0b0001);                                         \
    }                                                                                             \
    while (0)

#define SET_VEC_Y(v, y)                                                                           \
    do {                                                                                          \
        __m128 tmp_0 = _mm_set1_ps(y);                                                            \
        (v)->vec = _mm_blend_ps((v)->vec, tmp_0, 0b0010);                                         \
    }                                                                                             \
    while (0)

#define SET_VEC_Z(v, z)                                                                           \
    do {                                                                                          \
        __m128 tmp_0 = _mm_set1_ps(z);                                                            \
        (v)->vec = _mm_blend_ps((v)->vec, tmp_0, 0b0100);                                         \
    }                                                                                             \
    while (0)

#define SIGN(v) \
    (-((GET_VEC_X(v) < 0) || (GET_VEC_Y(v) < 0) || (GET_VEC_Z(v) < 0)))

vec3f add_scalar(vec3f a, f32 scalar);

vec3f add_vec(vec3f a, vec3f b);

vec3f sub_scalar(vec3f a, f32 scalar);

vec3f sub_vec(vec3f a, vec3f b);

vec3f mul_scalar(vec3f a, f32 scalar);

vec3f mul_vec(vec3f a, vec3f b);

vec3f div_scalar(vec3f a, f32 scalar);

f32 length(vec3f a);

vec3f normalize(vec3f a);

f32 dot(vec3f a, vec3f b);

vec3f cross(vec3f a, vec3f b);

vec3f inverse(vec3f vec);

vec3f min_vec(vec3f a, vec3f b);

vec3f max_vec(vec3f a, vec3f b);

vec3f refract(vec3f i, vec3f n, f32 eta);

vec3f reflect(vec3f i, vec3f n);

C_GUARD_END()

#endif //SOFTWARE_RAYTRACING_VEC3_H
