//
// Created by Luis Ruisinger on 06.01.25.
//

#include "vec4.h"

//
// Created by Luis Ruisinger on 24.11.24.
//

#include "vec4.h"

vec4f vec4_add(vec4f a, vec4f b) {
    return (vec4f) { .vec = _mm_add_ps(a.vec, b.vec) };
}

vec4f vec4_adds(vec4f a, f32 scalar) {
    __m128 vec_b = _mm_load1_ps(&scalar);
    return vec4_add(a, *(vec4f *) &vec_b);
}

vec4f vec4_sub(vec4f a, vec4f b) {
    return (vec4f) { .vec =  _mm_sub_ps(a.vec, b.vec) };
}

vec4f vec4_subs(vec4f a, f32 scalar) {
    __m128 vec_b = _mm_load1_ps(&scalar);
    return vec4_sub(a, *(vec4f *) &vec_b);
}

vec4f vec4_mul(vec4f a, vec4f b) {
    return (vec4f) { .vec = _mm_mul_ps(a.vec, b.vec) };
}

vec4f vec4_muls(vec4f a, f32 scalar) {
    __m128 vec_b = _mm_load1_ps(&scalar);
    return vec4_mul(a, *(vec4f *) &vec_b);
}

vec4f vec4_divs(vec4f a, f32 scalar) {
    return vec4_muls(a, 1.0F / scalar);
}

f32 vec4_length(vec4f a) {
    a.vec = _mm_mul_ps(a.vec, a.vec);
    a.vec = _mm_hadd_ps(a.vec, a.vec);
    a.vec = _mm_hadd_ps(a.vec, a.vec);

    return sqrtf(_mm_cvtss_f32(a.vec));
}

vec4f vec4_normalize(vec4f a) {
    __m128 _tmp_0 = _mm_mul_ps(a.vec, a.vec);
    _tmp_0 = _mm_hadd_ps(_tmp_0, _tmp_0);
    _tmp_0 = _mm_hadd_ps(_tmp_0, _tmp_0);
    _tmp_0 = _mm_rsqrt_ss(_tmp_0);
    _tmp_0 = _mm_shuffle_ps(_tmp_0, _tmp_0, _MM_SHUFFLE(0, 0, 0, 0));
    _tmp_0 = _mm_mul_ps(a.vec, _tmp_0);

    return (vec4f) { .vec = _tmp_0 };
}

f32 vec4_dot(vec4f a, vec4f b) {
    b.vec = _mm_mul_ps(a.vec, b.vec);
    b.vec = _mm_hadd_ps(b.vec, b.vec);
    b.vec = _mm_hadd_ps(b.vec, b.vec);

    return _mm_cvtss_f32(b.vec);
}

vec4f vec4_inverse(vec4f a) {
    __m128 _tmp_0 = _mm_set1_ps(0.0F);
    __m128 _tmp_1 = _mm_cmp_ps(a.vec, _tmp_0, _CMP_EQ_OQ);
    __m128 _tmp_2 = _mm_blendv_ps(a.vec, _mm_set1_ps(INFINITY), _tmp_1);

    return (vec4f) { .vec = _tmp_2 };
}

vec4f vec4_min(vec4f a, vec4f b) {
    return (vec4f) { .vec = _mm_min_ps(a.vec, b.vec) };
}

vec4f vec4_max(vec4f a, vec4f b) {
    return (vec4f) { .vec = _mm_max_ps(a.vec, b.vec) };
}
