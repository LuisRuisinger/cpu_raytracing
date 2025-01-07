//
// Created by Luis Ruisinger on 06.01.25.
//

#ifndef CPU_RAYTRACING_MATRIX_H
#define CPU_RAYTRACING_MATRIX_H

#include "../defines.h"
#include "vec3.h"

C_GUARD_BEGINN()

typedef struct Mat2x2_t {
    union {
        alignas(16) f32 val[4];
        __m128 row;
    };
} Mat2x2;

typedef struct Mat4x4_t {
    union {
        alignas(16) f32 val[16];
        __m128 rows[4];
    };
} Mat4x4;

#define ROW(mat, i) \
    *((__m128*) &(mat)->val + i)

ALWAYS_INLINE static Mat2x2 mat4x4_to_mat2x2(const Mat4x4 *__restrict__ src) {
    return (Mat2x2) { .row = _mm_movelh_ps(ROW(src, 0), ROW(src, 1)) };
}

ALWAYS_INLINE static void mat2x2_mulm(
        const Mat2x2 *__restrict__ a, const Mat2x2 *__restrict__ b, Mat2x2 *__restrict__ c) {
    __m128 _tmp_0 = _mm_shuffle_epi32(_mm_castps_si128(ROW(b, 0)), _MM_SHUFFLE(3, 0, 3, 0));
    __m128 _tmp_1 = _mm_shuffle_epi32(_mm_castps_si128(ROW(a, 0)), _MM_SHUFFLE(2, 3, 0, 1));
    __m128 _tmp_2 = _mm_shuffle_epi32(_mm_castps_si128(ROW(b, 0)), _MM_SHUFFLE(1, 2, 1, 2));

    __m128 _tmp_3 = _mm_mul_ps(ROW(a, 0), _mm_castsi128_ps(_tmp_0));
    __m128 _tmp_4 = _mm_mul_ps(_mm_castsi128_ps(_tmp_1), _mm_castsi128_ps(_tmp_2));

    ROW(c, 0) = _mm_add_ps(_tmp_3, _tmp_4);
}

ALWAYS_INLINE static void mat2x2_transpose(const Mat2x2 *__restrict__ src, Mat2x2 *__restrict__ dst) {
    ROW(dst, 0) = _mm_shuffle_ps(ROW(src, 0), ROW(src, 0), _MM_SHUFFLE(0, 2, 1, 3));
}

ALWAYS_INLINE static void mat2x2_adjmul(
        const Mat2x2 *__restrict__ a, const Mat2x2 *__restrict__ b, Mat2x2 *__restrict__ c) {
    __m128 _tmp_0 = _mm_shuffle_epi32(_mm_castps_si128(ROW(a, 0)), _MM_SHUFFLE(0, 3, 0, 3));
    __m128 _tmp_1 = _mm_shuffle_epi32(_mm_castps_si128(ROW(a, 0)), _MM_SHUFFLE(2, 2, 1, 1));
    __m128 _tmp_2 = _mm_shuffle_epi32(_mm_castps_si128(ROW(b, 0)), _MM_SHUFFLE(1, 0, 3, 2));

    __m128 _tmp_3 = _mm_mul_ps(_mm_castsi128_ps(_tmp_0), ROW(b, 0));
    __m128 _tmp_4 = _mm_mul_ps(_mm_castsi128_ps(_tmp_1), _mm_castsi128_ps(_tmp_2));

    ROW(c, 0) = _mm_sub_ps(_tmp_3, _tmp_4);
}

ALWAYS_INLINE static void mat2x2_muladj(
        const Mat2x2 *__restrict__ a, const Mat2x2 *__restrict__ b, Mat2x2 *__restrict__ c) {
    __m128 _tmp_0 = _mm_shuffle_epi32(_mm_castps_si128(ROW(b, 0)), _MM_SHUFFLE(0, 3, 0, 3));
    __m128 _tmp_1 = _mm_shuffle_epi32(_mm_castps_si128(ROW(a, 0)), _MM_SHUFFLE(2, 3, 0, 1));
    __m128 _tmp_2 = _mm_shuffle_epi32(_mm_castps_si128(ROW(b, 0)), _MM_SHUFFLE(1, 2, 1, 2));

    __m128 _tmp_3 = _mm_mul_ps(ROW(a, 0), _mm_castsi128_ps(_tmp_0));
    __m128 _tmp_4 = _mm_mul_ps(_mm_castsi128_ps(_tmp_1), _mm_castsi128_ps( _tmp_2));

    ROW(c, 0) = _mm_sub_ps(_tmp_3, _tmp_4);
}

void mat2x2_inverse(const Mat2x2 *__restrict__ src, Mat2x2 *__restrict__ dst);
vec4f mat4x4_mulv(const Mat4x4 *mat, vec4f v);
void mat4x4_muls(const Mat4x4 *mat, Mat4x4 *dst, f32 s);
void mat4x4_transpose(const Mat4x4 *src, Mat4x4 *dst);
void mat4x4_mulm(const Mat4x4 *a, const Mat4x4 *b, Mat4x4 *c);

ALWAYS_INLINE static Mat2x2 mat2x2_indentity() {
    return (Mat2x2) { .row = _mm_set_ps(1.0F, 0.0F, 0.0F, 1.0F) };
}

ALWAYS_INLINE static Mat2x2 mat4x4_indentity() {
    /* TODO */
}

ALWAYS_INLINE static Mat4x4 mat2x2_to_mat4x4(const Mat2x2 *__restrict__ src) {
    /* TODO */
}

ALWAYS_INLINE static Mat4x4 mat4x4_from_vec(const __m128 vec[4]) {
    Mat4x4 src = { .rows = { vec[0], vec[1], vec[2], vec[3] } };
    Mat4x4 dst;

    mat4x4_transpose(&src, &dst);
    return dst;
}

C_GUARD_END()

#endif //CPU_RAYTRACING_MATRIX_H
