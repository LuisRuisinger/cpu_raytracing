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
        alignas(32) f32 val[4];
        alignas(32) __m128 row;
    };
} __attribute__((aligned(16))) Mat2x2;

typedef struct Mat4x4_t {
    union {
        alignas(32) f32 val[16];
        alignas(32) __m128 rows[4];
    };
} __attribute__((aligned(32))) Mat4x4;

#define ROW_128(mat, i) \
    *((__m128*) &(mat)->val + i)

#ifdef __AVX2__
#define ROW_256(mat, i) \
    *((__m256*) &(mat)->val + i)
#endif

#ifdef __AVX512F__
#define ROW_512(mat, i) \
    *((__m512*) &(mat)->val + i)
#endif

ALWAYS_INLINE static Mat2x2 mat2x2_indentity() {

    // this does not care about row-major / column-major ordering
    return (Mat2x2) { .row = _mm_set_ps(1.0F, 0.0F, 0.0F, 1.0F) };
}

ALWAYS_INLINE static Mat2x2 mat4x4_indentity() {
    /* TODO */
}

ALWAYS_INLINE static void mat2x2_mulm(
        const Mat2x2 *__restrict__ a, const Mat2x2 *__restrict__ b, Mat2x2 *__restrict__ c) {
    __m128 _tmp_0 = _mm_shuffle_epi32(_mm_castps_si128(ROW_128(b, 0)), _MM_SHUFFLE(3, 0, 3, 0));
    __m128 _tmp_1 = _mm_shuffle_epi32(_mm_castps_si128(ROW_128(a, 0)), _MM_SHUFFLE(2, 3, 0, 1));
    __m128 _tmp_2 = _mm_shuffle_epi32(_mm_castps_si128(ROW_128(b, 0)), _MM_SHUFFLE(1, 2, 1, 2));

    __m128 _tmp_3 = _mm_mul_ps(ROW_128(a, 0), _mm_castsi128_ps(_tmp_0));
    __m128 _tmp_4 = _mm_mul_ps(_mm_castsi128_ps(_tmp_1), _mm_castsi128_ps(_tmp_2));

    ROW_128(c, 0) = _mm_add_ps(_tmp_3, _tmp_4);
}

ALWAYS_INLINE static void mat2x2_transpose(
        const Mat2x2 *__restrict__ src, Mat2x2 *__restrict__ dst) {
    ROW_128(dst, 0) = _mm_shuffle_ps(ROW_128(src, 0), ROW_128(src, 0), _MM_SHUFFLE(0, 2, 1, 3));
}

ALWAYS_INLINE static void mat2x2_adjmul(
        const Mat2x2 *__restrict__ a, const Mat2x2 *__restrict__ b, Mat2x2 *__restrict__ c) {
    __m128 _tmp_0 = _mm_shuffle_epi32(_mm_castps_si128(ROW_128(a, 0)), _MM_SHUFFLE(0, 3, 0, 3));
    __m128 _tmp_1 = _mm_shuffle_epi32(_mm_castps_si128(ROW_128(a, 0)), _MM_SHUFFLE(2, 2, 1, 1));
    __m128 _tmp_2 = _mm_shuffle_epi32(_mm_castps_si128(ROW_128(b, 0)), _MM_SHUFFLE(1, 0, 3, 2));

    __m128 _tmp_3 = _mm_mul_ps(_mm_castsi128_ps(_tmp_0), ROW_128(b, 0));
    __m128 _tmp_4 = _mm_mul_ps(_mm_castsi128_ps(_tmp_1), _mm_castsi128_ps(_tmp_2));

    ROW_128(c, 0) = _mm_sub_ps(_tmp_3, _tmp_4);
}

ALWAYS_INLINE static void mat2x2_muladj(
        const Mat2x2 *__restrict__ a, const Mat2x2 *__restrict__ b, Mat2x2 *__restrict__ c) {
    __m128 _tmp_0 = _mm_shuffle_epi32(_mm_castps_si128(ROW_128(b, 0)), _MM_SHUFFLE(0, 3, 0, 3));
    __m128 _tmp_1 = _mm_shuffle_epi32(_mm_castps_si128(ROW_128(a, 0)), _MM_SHUFFLE(2, 3, 0, 1));
    __m128 _tmp_2 = _mm_shuffle_epi32(_mm_castps_si128(ROW_128(b, 0)), _MM_SHUFFLE(1, 2, 1, 2));

    __m128 _tmp_3 = _mm_mul_ps(ROW_128(a, 0), _mm_castsi128_ps(_tmp_0));
    __m128 _tmp_4 = _mm_mul_ps(_mm_castsi128_ps(_tmp_1), _mm_castsi128_ps( _tmp_2));

    ROW_128(c, 0) = _mm_sub_ps(_tmp_3, _tmp_4);
}

ALWAYS_INLINE static void mat4x4_muls(
        const Mat4x4 *__restrict__ mat, Mat4x4 *__restrict__ dst, f32 s) {
#ifdef __AVX2__
    __m256 _tmp_0 = _mm256_set1_ps(s);

    // this does not care about row-major / column-major ordering
    ROW_256(dst, 0) = _mm256_mul_ps(ROW_256(mat, 0), _tmp_0);
    ROW_256(dst, 1) = _mm256_mul_ps(ROW_256(mat, 1), _tmp_0);
#else
    __m128 _tmp_0 = _mm_set1_ps(s);

    // this does not care about row-major / column-major ordering
    ROW_128(dst, 0) = _mm_mul_ps(ROW_128(mat, 0), _tmp_0);
    ROW_128(dst, 1) = _mm_mul_ps(ROW_128(mat, 1), _tmp_0);
    ROW_128(dst, 2) = _mm_mul_ps(ROW_128(mat, 2), _tmp_0);
    ROW_128(dst, 3) = _mm_mul_ps(ROW_128(mat, 3), _tmp_0);
#endif
}

ALWAYS_INLINE static void mat4x4_transpose(
        const Mat4x4 *__restrict__ src, Mat4x4 *__restrict__ dst) {
    __m128 _tmp_0 = _mm_shuffle_ps(ROW_128(src, 0), ROW_128(src, 1), 0x44);
    __m128 _tmp_1 = _mm_shuffle_ps(ROW_128(src, 0), ROW_128(src, 1), 0xEE);
    __m128 _tmp_2 = _mm_shuffle_ps(ROW_128(src, 2), ROW_128(src, 3), 0x44);
    __m128 _tmp_3 = _mm_shuffle_ps(ROW_128(src, 2), ROW_128(src, 3), 0xEE);

    ROW_128(dst, 0) = _mm_shuffle_ps(_tmp_0, _tmp_2, 0x88);
    ROW_128(dst, 1) = _mm_shuffle_ps(_tmp_0, _tmp_2, 0xDD);
    ROW_128(dst, 2) = _mm_shuffle_ps(_tmp_1, _tmp_3, 0x88);
    ROW_128(dst, 3) = _mm_shuffle_ps(_tmp_1, _tmp_3, 0xDD);
}

ALWAYS_INLINE static vec4f mat4x4_mulv(const Mat4x4 *__restrict__ mat, vec4f v) {
#ifdef __AVX512F__
#elif __AVX2__
    __m256 _tmp_0 = _mm256_set_m128(_mm_set1_ps(GET_VEC4_X(v)), _mm_set1_ps(GET_VEC4_Y(v)));
    __m256 _tmp_1 = _mm256_set_m128(_mm_set1_ps(GET_VEC4_Z(v)), _mm_set1_ps(GET_VEC4_W(v)));
    __m256 _tmp_2 = _mm256_mul_ps(ROW_256(mat, 0), _tmp_0);
    __m256 _tmp_3 = _mm256_mul_ps(ROW_256(mat, 1), _tmp_1);
    __m256 _tmp_4 = _mm256_add_ps(_tmp_2, _tmp_3);

    __m128 _tmp_5 = _mm_add_ps(_mm256_extractf128_ps(_tmp_4, 1), _mm256_castps256_ps128(_tmp_4));
    return (vec4f) { .vec = _tmp_5 };

#else
    __m128 _tmp_0 = _mm_mul_ps(ROW_128(mat, 0), _mm_set1_ps(GET_VEC4_X(v)));
    __m128 _tmp_1 = _mm_mul_ps(ROW_128(mat, 1), _mm_set1_ps(GET_VEC4_Y(v)));
    __m128 _tmp_2 = _mm_mul_ps(ROW_128(mat, 2), _mm_set1_ps(GET_VEC4_Z(v)));
    __m128 _tmp_3 = _mm_mul_ps(ROW_128(mat, 3), _mm_set1_ps(GET_VEC4_W(v)));

    __m128 _tmp_4 = _mm_add_ps(_mm_add_ps(_tmp_0, _tmp_1), _mm_add_ps(_tmp_2, _tmp_3));
    return (vec4f) { .vec = _tmp_4 };

#endif
}

#ifdef __AVX512F__
ALWAYS_INLINE static __m512 mat4x4_mulv4(const Mat4x4 *__restrict__ mat, __m512 pv) {
    __m512 _tmp_0 = _mm512_shuffle_epi32(_mm512_castps_si512(pv), _MM_SHUFFLE(0, 0, 0, 0));
    __m512 _tmp_1 = _mm512_shuffle_epi32(_mm512_castps_si512(pv), _MM_SHUFFLE(1, 1, 1, 1));
    __m512 _tmp_2 = _mm512_shuffle_epi32(_mm512_castps_si512(pv), _MM_SHUFFLE(2, 2, 2, 2));
    __m512 _tmp_3 = _mm512_shuffle_epi32(_mm512_castps_si512(pv), _MM_SHUFFLE(3, 3, 3, 3));

    __m512 _tmp_4 = _mm512_mul_ps(ROW_512(mat, 0), _tmp_0);
    __m512 _tmp_5 = _mm512_mul_ps(ROW_512(mat, 1), _tmp_1);
    __m512 _tmp_6 = _mm512_mul_ps(ROW_512(mat, 2), _tmp_2);
    __m512 _tmp_7 = _mm512_mul_ps(ROW_512(mat, 3), _tmp_3);

    return _mm512_add_ps(_mm512_add_ps(_tmp_4, _tmp_5), _mm512_add_ps(_tmp_6, _tmp_7));
}
#endif

#ifdef __AVX2__
ALWAYS_INLINE static __m256 mat4x4_mulv2(const Mat4x4 *__restrict__ mat, __m256 pv) {
    __m256 _tmp_0 = _mm256_shuffle_epi32(_mm256_castps_si256(pv), _MM_SHUFFLE(0, 0, 0, 0));
    __m256 _tmp_1 = _mm256_shuffle_epi32(_mm256_castps_si256(pv), _MM_SHUFFLE(1, 1, 1, 1));
    __m256 _tmp_2 = _mm256_shuffle_epi32(_mm256_castps_si256(pv), _MM_SHUFFLE(2, 2, 2, 2));
    __m256 _tmp_3 = _mm256_shuffle_epi32(_mm256_castps_si256(pv), _MM_SHUFFLE(3, 3, 3, 3));

    __m256 _tmp_4 = _mm256_mul_ps(ROW_256(mat, 0), _tmp_0);
    __m256 _tmp_5 = _mm256_mul_ps(ROW_256(mat, 1), _tmp_1);
    __m256 _tmp_6 = _mm256_mul_ps(ROW_256(mat, 2), _tmp_2);
    __m256 _tmp_7 = _mm256_mul_ps(ROW_256(mat, 3), _tmp_3);

    return _mm256_add_ps(_mm256_add_ps(_tmp_4, _tmp_5), _mm256_add_ps(_tmp_6, _tmp_7));
}
#endif

ALWAYS_INLINE static void mat4x4_mulm(
        const Mat4x4 *__restrict__ a, const Mat4x4 *__restrict__ b, Mat4x4 *__restrict__ c) {

    // 4x4 matrices are stored in row-major order rather than column-major to speed up
    // 4x4 matrix vector and matrix multiplication
    // this implies that b itself doesn't need to be transposed to linearize the multiplication
#ifdef __AVX512F__
    ROW_512(c, 0) = mat4x4_mulv4(a, *((__m512 *) b->val + 0));

#elif __AVX2__
    ROW_256(c, 0) = mat4x4_mulv2(a, *((__m256 *) b->val + 0));
    ROW_256(c, 1) = mat4x4_mulv2(a, *((__m256 *) b->val + 1));

#else
    ROW_128(c, 0) = mat4x4_mulv(a, *((vec4f *) b->val + 0)).vec;
    ROW_128(c, 1) = mat4x4_mulv(a, *((vec4f *) b->val + 1)).vec;
    ROW_128(c, 2) = mat4x4_mulv(a, *((vec4f *) b->val + 2)).vec;
    ROW_128(c, 3) = mat4x4_mulv(a, *((vec4f *) b->val + 3)).vec;

#endif
}

ALWAYS_INLINE static Mat4x4 mat4x4_from_vec(const __m128 vec[4]) {

    // 4x4 matrices are stored in row-major order rather than column-major to speed up
    // 4x4 matrix vector and matrix multiplication
    return (Mat4x4) { .rows = { vec[0], vec[1], vec[2], vec[3] } };
}

ALWAYS_INLINE static Mat2x2 mat4x4_to_mat2x2(const Mat4x4 *__restrict__ src) {
    Mat2x2 m = { .row = _mm_movelh_ps(ROW_128(src, 0), ROW_128(src, 1)) };
    Mat2x2 dst;

    // 2x2 matrices are stored in column-major order in contrast to
    // the row-major representation of 4x4 matrices
    mat2x2_transpose(&m, &dst);
    return dst;
}

ALWAYS_INLINE static Mat4x4 mat2x2_to_mat4x4(const Mat2x2 *__restrict__ src) {
    Mat2x2 m;

    // 2x2 matrices are stored in column-major order in contrast to
    // the row-major representation of 4x4 matrices
    mat2x2_transpose(src, &m);


}

/**
 * @brief Computes the inverse of an arbitrary 2x2 matrix in columm-major order
 * @param src The source transformation matrix.
 * @param dst The destination transformation matrix where we want to store the inverse of src.
 */
void mat2x2_inverse(const Mat2x2 *__restrict__ src, Mat2x2 *__restrict__ dst);

/**
 * @brief Computes the inverse of an arbitrary 4x4 matrix in row-major order
 * @param src The source transformation matrix.
 * @param dst The destination transformation matrix where we want to store the inverse of src.
 */
void mat4x4_inverse(const Mat4x4 *__restrict__ src, Mat4x4 *__restrict__ dst);

/**
 * @brief Computes the inverse of a transformation matrix in row-major order
 *        which does not apply any scaling.
 * @param src The source transformation matrix.
 * @param dst The destination transformation matrix where we want to store the inverse of src.
 */
void mat4x4_inverse_tns(const Mat4x4 *__restrict__ src, Mat4x4 *__restrict__ dst);

/**
 * @brief Computes the inverse of a general transformation matrix in row-major order
 * @param src The source transformation matrix.
 * @param dst The destination transformation matrix where we want to store the inverse of src.
 */
void mat4x4_inverse_t(const Mat4x4 *__restrict__ src, Mat4x4 *__restrict__ dst);

C_GUARD_END()

#endif //CPU_RAYTRACING_MATRIX_H
