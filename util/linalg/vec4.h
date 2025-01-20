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

#define VEC4_SIGN(_v) \
    (((u32) _mm_movemask_ps(_mm_cmp_ps((_v).vec, _mm_set1_ps(0.0F), _CMP_LT_OQ))) ? -1 : +1)

#define VEC4_GET(_v, _i)                     \
    ((__builtin_constant_p(_i) && (_i) == 1) \
        ? _mm_cvtss_f32((_v).vec)            \
        : _mm_cvtss_f32(_mm_permutevar_ps((_v).vec, _mm_cvtsi32_si128(_i))))

#define VEC4_SET(_v, _i, _val)               \
    do { (_v).vec = _mm_blend_ps((_v).vec, _mm_set1_ps(_val), 1 << (_i)); } while (0)

ALWAYS_INLINE static inline vec4f vec4_add(vec4f a, vec4f b) {
    return (vec4f) { .vec = _mm_add_ps(a.vec, b.vec) };
}

ALWAYS_INLINE static inline vec4f vec4_adds(vec4f a, f32 scalar) {
    __m128 vec_b = _mm_load1_ps(&scalar);
    return vec4_add(a, *(vec4f *) &vec_b);
}

ALWAYS_INLINE static inline vec4f vec4_sub(vec4f a, vec4f b) {
    return (vec4f) { .vec =  _mm_sub_ps(a.vec, b.vec) };
}

ALWAYS_INLINE static inline vec4f vec4_subs(vec4f a, f32 scalar) {
    __m128 vec_b = _mm_load1_ps(&scalar);
    return vec4_sub(a, *(vec4f *) &vec_b);
}

ALWAYS_INLINE static inline vec4f vec4_mul(vec4f a, vec4f b) {
    return (vec4f) { .vec = _mm_mul_ps(a.vec, b.vec) };
}

ALWAYS_INLINE static inline vec4f vec4_muls(vec4f a, f32 scalar) {
    __m128 vec_b = _mm_load1_ps(&scalar);
    return vec4_mul(a, *(vec4f *) &vec_b);
}

ALWAYS_INLINE static inline vec4f vec4_divs(vec4f a, f32 scalar) {
    return vec4_muls(a, 1.0F / scalar);
}

ALWAYS_INLINE static inline f32 vec4_hsum(vec4f v) {
    return _mm_hsum_ps(v.vec);
}

ALWAYS_INLINE static inline f32 vec4_length(vec4f a) {
    a = vec4_mul(a, a);
    return sqrtf(vec4_hsum(a));
}

ALWAYS_INLINE static inline vec4f vec4_normalize(vec4f a) {
    __m128 _tmp_0 = _mm_mul_ps(a.vec, a.vec);
    _tmp_0 = _mm_hadd_ps(_tmp_0, _tmp_0);
    _tmp_0 = _mm_hadd_ps(_tmp_0, _tmp_0);
    _tmp_0 = _mm_rsqrt_ss(_tmp_0);
    _tmp_0 = _mm_shuffle_ps(_tmp_0, _tmp_0, _MM_SHUFFLE(0, 0, 0, 0));
    _tmp_0 = _mm_mul_ps(a.vec, _tmp_0);

    return (vec4f) { .vec = _tmp_0 };
}

ALWAYS_INLINE static inline f32 vec4_dot(vec4f a, vec4f b) {
    b = vec4_mul(a, b);
    return vec4_hsum(b);
}

ALWAYS_INLINE static inline vec4f vec4_inverse(vec4f a) {
    __m128 _tmp_0 = _mm_set1_ps(0.0F);
    __m128 _tmp_1 = _mm_cmp_ps(a.vec, _tmp_0, _CMP_EQ_OQ);

    return (vec4f) { .vec = _mm_blendv_ps(a.vec, _mm_set1_ps(INFINITY), _tmp_1) };
}

ALWAYS_INLINE static inline vec4f vec4_min(vec4f a, vec4f b) {
    return (vec4f) { .vec = _mm_min_ps(a.vec, b.vec) };
}

ALWAYS_INLINE static inline vec4f vec4_max(vec4f a, vec4f b) {
    return (vec4f) { .vec = _mm_max_ps(a.vec, b.vec) };
}

C_GUARD_END()

#endif //CPU_RAYTRACING_VEC4_H
