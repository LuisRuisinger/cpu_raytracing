//
// Created by Luis Ruisinger on 24.11.24.
//

#include "vec3.h"
#include "vec4.h"

vec3f vec3_add(vec3f a, vec3f b) {
    return vec4_add(a, b);
}

vec3f vec3_adds(vec3f a, f32 scalar) {
    a = vec4_adds(a, scalar);
    SET_VEC4_W(a, 0.0F);

    return a;
}

vec3f vec3_sub(vec3f a, vec3f b) {
    return vec4_sub(a, b);
}

vec3f vec3_subs(vec3f a, f32 scalar) {
    a = vec4_subs(a, scalar);
    SET_VEC4_W(a, 0.0F);

    return a;
}

vec3f vec3_mul(vec3f a, vec3f b) {
    return vec4_mul(a, b);
}

vec3f vec3_muls(vec3f a, f32 scalar) {
    return vec4_muls(a, scalar);
}

vec3f vec3_divs(vec3f a, f32 scalar) {
    a = vec4_divs(a, scalar);
    SET_VEC4_W(a, 0.0F);

    return a;
}

f32 vec3_length(vec3f a) {
    return vec4_length(a);
}

vec3f vec3_normalize(vec3f a) {
    return vec4_normalize(a);
}

f32 vec3_dot(vec3f a, vec3f b) {
    return vec4_dot(a, b);
}

vec3f vec3_cross(vec3f a, vec3f b) {
    __m128 _tmp_0 = _mm_shuffle_ps(a.vec, a.vec, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 _tmp_1 = _mm_shuffle_ps(b.vec, b.vec, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 _tmp_2 = _mm_mul_ps(_tmp_0, b.vec);
    __m128 _tmp_3 = _mm_mul_ps(_tmp_0, _tmp_1);
    __m128 _tmp_4 = _mm_shuffle_ps(_tmp_2, _tmp_2, _MM_SHUFFLE(3, 0, 2, 1));

    return (vec3f) { .vec = _mm_sub_ps(_tmp_3, _tmp_4) };
}

vec3f vec3_inverse(vec3f a) {
    a = vec4_inverse(a);
    SET_VEC4_W(a, 0.0F);

    return a;
}

vec3f vec3_min(vec3f a, vec3f b) {
    return (vec3f) { .vec = _mm_min_ps(a.vec, b.vec) };
}

vec3f vec3_max(vec3f a, vec3f b) {
    return (vec3f) { .vec = _mm_max_ps(a.vec, b.vec) };
}

vec3f vec3_refract(vec3f i, vec3f n, f32 eta) {
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

vec3f vec3_reflect(vec3f i, vec3f n) {
    f32 tmp_0 = 2.0F * vec3_dot(n, i);
    n = vec3_muls(n, tmp_0);

    return vec3_sub(i, n);
}
