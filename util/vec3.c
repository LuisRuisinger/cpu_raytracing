//
// Created by Luis Ruisinger on 24.11.24.
//

#include "vec3.h"

vec3f add_vec(vec3f a, vec3f b) {
    return (vec3f) {
        .vec = _mm_add_ps(a.vec, b.vec)
    };
}

vec3f add_scalar(vec3f a, f32 scalar) {
    __m128 vec_b = _mm_load1_ps(&scalar);
    return add_vec(a, *(vec3f *) &vec_b);
}

vec3f sub_vec(vec3f a, vec3f b) {
    return (vec3f) {
        .vec =  _mm_sub_ps(a.vec, b.vec)
    };
}

vec3f sub_scalar(vec3f a, f32 scalar) {
    __m128 vec_b = _mm_load1_ps(&scalar);
    return sub_vec(a, *(vec3f *) &vec_b);
}

vec3f mul_vec(vec3f a, vec3f b) {
    return (vec3f) {
        .vec = _mm_mul_ps(a.vec, b.vec)
    };
}

vec3f mul_scalar(vec3f a, f32 scalar) {
    __m128 vec_b = _mm_load1_ps(&scalar);
    return mul_vec(a, *(vec3f *) &vec_b);
}

vec3f div_scalar(vec3f a, f32 scalar) {
    return mul_scalar(a, 1.0F / scalar);
}

f32 length(vec3f a) {
    a.vec = _mm_mul_ps(a.vec, a.vec);
    a.vec = _mm_hadd_ps(a.vec, a.vec);
    a.vec = _mm_hadd_ps(a.vec, a.vec);

    return sqrtf(_mm_cvtss_f32(a.vec));
}

vec3f normalize(vec3f a) {
    __m128 _tmp_0 = _mm_mul_ps(a.vec, a.vec);
    _tmp_0 = _mm_hadd_ps(_tmp_0, _tmp_0);
    _tmp_0 = _mm_hadd_ps(_tmp_0, _tmp_0);
    _tmp_0 = _mm_rsqrt_ss(_tmp_0);
    _tmp_0 = _mm_shuffle_ps(_tmp_0, _tmp_0, _MM_SHUFFLE(0, 0, 0, 0));
    _tmp_0 = _mm_mul_ps(a.vec, _tmp_0);

    return (vec3f) {
        .vec = _tmp_0
    };
}

f32 dot(vec3f a, vec3f b) {
    b.vec = _mm_mul_ps(a.vec, b.vec);
    b.vec = _mm_hadd_ps(b.vec, b.vec);
    b.vec = _mm_hadd_ps(b.vec, b.vec);

    return _mm_cvtss_f32(b.vec);
}

vec3f cross(vec3f a, vec3f b) {
    __m128 _tmp_0 = _mm_shuffle_ps(a.vec, a.vec, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 _tmp_1 = _mm_shuffle_ps(b.vec, b.vec, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 _tmp_2 = _mm_mul_ps(_tmp_0, b.vec);
    __m128 _tmp_3 = _mm_mul_ps(_tmp_0, _tmp_1);
    __m128 _tmp_4 = _mm_shuffle_ps(_tmp_2, _tmp_2, _MM_SHUFFLE(3, 0, 2, 1));

    return (vec3f) {
        .vec = _mm_sub_ps(_tmp_3, _tmp_4)
    };
}

vec3f inverse(vec3f a) {
    f32 x = GET_VEC3_X(a);
    f32 y = GET_VEC3_Y(a);
    f32 z = GET_VEC3_Z(a);

    return VEC3(
            x != 0 ? 1.0F / x : INFINITY,
            y != 0 ? 1.0F / y : INFINITY,
            z != 0 ? 1.0F / z : INFINITY);
}

vec3f min_vec(vec3f a, vec3f b) {
    return (vec3f) {
        .vec = _mm_min_ps(a.vec, b.vec)
    };
}

vec3f max_vec(vec3f a, vec3f b) {
    return (vec3f) {
        .vec = _mm_max_ps(a.vec, b.vec)
    };
}

vec3f refract(vec3f i, vec3f n, f32 eta) {
    f32 tmp_0 = dot(n, i);
    f32 k = 1.0F - eta * eta * (1.0F - tmp_0 * tmp_0);

    if (k < 0.0F) {
        return VEC3(0.0F);
    }

    f32 tmp_1 = eta * tmp_0 + sqrtf(k);
    n = mul_scalar(n, tmp_1);
    i = mul_scalar(i, eta);

    return sub_vec(i, n);
}

vec3f reflect(vec3f i, vec3f n) {
    f32 tmp_0 = 2.0F * dot(n, i);
    n = mul_scalar(n, tmp_0);

    return sub_vec(i, n);
}
