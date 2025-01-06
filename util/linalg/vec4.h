//
// Created by Luis Ruisinger on 06.01.25.
//

#ifndef CPU_RAYTRACING_VEC4_H
#define CPU_RAYTRACING_VEC4_H

#include "../defines.h"

C_GUARD_BEGINN()

typedef struct vec4f_t {
    __m128 vec;
} __attribute__((aligned(16))) vec4f;

#define VEC4_1(_x) \
    (vec4f) { .vec = _mm_set_ps(_x, _x, _x, _x) }

#define VEC4_2(_x, _y) \
    (vec4f) { .vec = _mm_set_ps(_t, _y, _y, _x) }

#define VEC4_3(_x, _y, _z) \
    (vec4f) { .vec = _mm_set_ps(_z, _z, _y, _x) }

#define VEC4_4(_x, _y, _z, _w) \
    (vec4f) { .vec = _mm_set_ps(_w, _z, _y, _x) }

#define VEC4_GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME

#define VEC4(...) \
    VEC4_GET_MACRO(__VA_ARGS__, VEC4_4, VEC4_3, VEC4_2, VEC4_1, INVALID)(__VA_ARGS__)

#define GET_VEC4_X(_v) \
    _mm_cvtss_f32((_v).vec)

#define GET_VEC4_Y(_v) \
    _mm_cvtss_f32(_mm_permutevar_ps((_v).vec, _mm_cvtsi32_si128(1)))

#define GET_VEC4_Z(_v) \
    _mm_cvtss_f32(_mm_permutevar_ps((_v).vec, _mm_cvtsi32_si128(2)))

#define GET_VEC4_W(_v) \
    _mm_cvtss_f32(_mm_permutevar_ps((_v).vec, _mm_cvtsi32_si128(3)))

#define SET_VEC4_X(_v, _x) \
    do { (_v).vec = _mm_blend_ps((_v).vec, _mm_set1_ps(_x), 1 << 0); } while (0)

#define SET_VEC4_Y(_v, _y) \
    do { (_v).vec = _mm_blend_ps((_v).vec, _mm_set1_ps(_y), 1 << 1); } while (0)

#define SET_VEC4_Z(_v, _z) \
    do { (_v).vec = _mm_blend_ps((_v).vec, _mm_set1_ps(_z), 1 << 2); } while (0)

#define SET_VEC4_W(_v, _w) \
    do { (_v).vec = _mm_blend_ps((_v).vec, _mm_set1_ps(_w), 1 << 3); } while (0)

#define VEC4_SIGN(_v) \
    (((u32) _mm_movemask_ps(_mm_cmp_ps((_v).vec, _mm_set1_ps(0.0F), _CMP_LT_OQ))) ? -1 : +1)

vec4f vec4_adds(vec4f a, f32 scalar);
vec4f vec4_add(vec4f a, vec4f b);
vec4f vec4_subs(vec4f a, f32 scalar);
vec4f vec4_sub(vec4f a, vec4f b);
vec4f vec4_muls(vec4f a, f32 scalar);
vec4f vec4_mul(vec4f a, vec4f b);
vec4f vec4_divs(vec4f a, f32 scalar);
f32 vec4_length(vec4f a);
vec4f vec4_normalize(vec4f a);
f32 vec4_dot(vec4f a, vec4f b);
vec4f vec4_cross(vec4f a, vec4f b);
vec4f vec4_inverse(vec4f a);
vec4f vec4_min(vec4f a, vec4f b);
vec4f vec4_max(vec4f a, vec4f b);
vec4f vec4_refract(vec4f i, vec4f n, f32 eta);
vec4f vec4_reflect(vec4f i, vec4f n);

C_GUARD_END()

#endif //CPU_RAYTRACING_VEC4_H
