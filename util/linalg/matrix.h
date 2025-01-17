//
// Created by Luis Ruisinger on 06.01.25.
//

#ifndef CPU_RAYTRACING_MATRIX_H
#define CPU_RAYTRACING_MATRIX_H

#include "../defines.h"
#include "vec3.h"
#include "vec2.h"

C_GUARD_BEGINN()

#if defined(__AVX512F__)
    #define MAT_ALIGNMENT 64
#elif defined(__AVX2__)
    #define MAT_ALIGNMENT 32
#else
    #define MAT_ALIGNMENT 16
#endif

typedef struct Mat2x2_t {
    union {
        alignas(MAT_ALIGNMENT) f32 val[4];
        alignas(MAT_ALIGNMENT) __m128 row;
    };
} __attribute__((aligned(MAT_ALIGNMENT))) Mat2x2;

typedef struct Mat4x4_t {
    union {
        alignas(MAT_ALIGNMENT) f32 val[16];
        alignas(MAT_ALIGNMENT) __m128 rows[4];
    };
} __attribute__((aligned(MAT_ALIGNMENT))) Mat4x4;

#if defined(__AVX512F__)
#define ROW_512(mat, i) \
    *((__m512*) &(mat)->val + i)
#endif

#if defined(__AVX2__)
#define ROW_256(mat, i) \
    *((__m256*) &(mat)->val + i)
#endif

#define ROW_128(mat, i) \
    *((__m128*) &(mat)->val + i)

ALWAYS_INLINE static Mat2x2 mat2x2_identity(void) {

    // this does not care about row-major / column-major ordering
    return (Mat2x2) { .row = _mm_set_ps(1.0F, 0.0F, 0.0F, 1.0F) };
}

ALWAYS_INLINE static Mat4x4 mat4x4_identity(void) {
#if defined(__AVX512F__)
    Mat4x4 mat;

    // this does not care about row-major / column-major ordering
    ROW_512(&mat, 0) = _mm512_set_ps(
            0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F,
            1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);

    return mat;

#elif defined(__AVX2__)
    Mat4x4 mat;

    // this does not care about row-major / column-major ordering
    ROW_256(&mat, 0) = _mm256_set_ps(0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F);
    ROW_256(&mat, 1) = _mm256_set_ps(1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);

    return mat;

#else
    Mat4x4 mat;

    // the matrix is aligned to __m128 therefore this cast is valid
    // this does not care about row-major / column-major ordering
    ROW_128(&mat, 0) = _mm_set_ps(0.0F, 0.0F, 0.0F, 1.0F);
    ROW_128(&mat, 1) = _mm_set_ps(0.0F, 0.0F, 1.0F, 0.0F);
    ROW_128(&mat, 2) = _mm_set_ps(0.0F, 1.0F, 0.0F, 0.0F);
    ROW_128(&mat, 0) = _mm_set_ps(1.0F, 0.0F, 0.0F, 0.0F);

    return mat;

#endif
}

ALWAYS_INLINE static void mat2x2_muls(
        const Mat2x2 *__restrict__ src, Mat2x2 *__restrict__ dst, f32 s) {
    ROW_128(dst, 0) = _mm_mul_ps(ROW_128(src, 0), _mm_set1_ps(s));
}

ALWAYS_INLINE static vec2f mat2x2_mulv(const Mat2x2 *__restrict__ src, vec2f v) {
    __m128 _tmp_0 = _mm_shuffle_ps(ROW_128(src, 0), ROW_128(src, 0), _MM_SHUFFLE(3, 1, 2, 0));
    __m128 _tmp_1 = _mm_set_ps(GET_VEC2_X(v), GET_VEC2_Y(v), GET_VEC2_X(v), GET_VEC2_Y(v));
    _tmp_1 = _mm_mul_ps(_tmp_0, _tmp_1);
    _tmp_1 = _mm_hadd_ps(_tmp_1, _tmp_1);

    return (vec2f) {
        .x = _mm_cvtss_f32(_tmp_1),
        .y = _mm_cvtss_f32(_mm_movehl_ps(_tmp_1, _tmp_1))
    };
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
    ROW_128(dst, 0) = _mm_shuffle_ps(ROW_128(src, 0), ROW_128(src, 0), _MM_SHUFFLE(3, 1, 2, 0));
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
        const Mat4x4 *__restrict__ src, Mat4x4 *__restrict__ dst, f32 s) {
#if defined(__AVX512F__)
    __m512 _tmp_0 = _mm512_set1_ps(s);

    // this does not care about row-major / column-major ordering
    ROW_512(dst, 0) = _mm512_mul_ps(ROW_512(src, 0), _tmp_0);

#elif defined(__AVX2__)
    __m256 _tmp_0 = _mm256_set1_ps(s);

    // this does not care about row-major / column-major ordering
    ROW_256(dst, 0) = _mm256_mul_ps(ROW_256(src, 0), _tmp_0);
    ROW_256(dst, 1) = _mm256_mul_ps(ROW_256(src, 1), _tmp_0);

#else
    __m128 _tmp_0 = _mm_set1_ps(s);

    // this does not care about row-major / column-major ordering
    ROW_128(dst, 0) = _mm_mul_ps(ROW_128(src, 0), _tmp_0);
    ROW_128(dst, 1) = _mm_mul_ps(ROW_128(src, 1), _tmp_0);
    ROW_128(dst, 2) = _mm_mul_ps(ROW_128(src, 2), _tmp_0);
    ROW_128(dst, 3) = _mm_mul_ps(ROW_128(src, 3), _tmp_0);
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

/* TODO test if the __AVX2__ variant is actually slower */
ALWAYS_INLINE static vec4f mat4x4_mulv(const Mat4x4 *__restrict__ mat, vec4f v) {
#if defined(__AVX2__)
    __m256 _tmp_0 = _mm256_set_m128(_mm_set1_ps(GET_VEC4_Y(v)), _mm_set1_ps(GET_VEC4_X(v)));
    __m256 _tmp_1 = _mm256_set_m128(_mm_set1_ps(GET_VEC4_W(v)), _mm_set1_ps(GET_VEC4_Z(v)));
    __m256 _tmp_2 = _mm256_mul_ps(ROW_256(mat, 0), _tmp_0);
    __m256 _tmp_3 = _mm256_mul_ps(ROW_256(mat, 1), _tmp_1);
    __m256 _tmp_4 = _mm256_add_ps(_tmp_2, _tmp_3);

    // combined multiplication for the first 2 rows and the last 2 rows
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

#if defined(__AVX2__)
ALWAYS_INLINE static __m256 mat4x4_mulv2(const Mat4x4 *__restrict__ mat, __m256 pv) {
    __m256 _tmp_0 = _mm256_shuffle_epi32(_mm256_castps_si256(pv), _MM_SHUFFLE(0, 0, 0, 0));
    __m256 _tmp_1 = _mm256_shuffle_epi32(_mm256_castps_si256(pv), _MM_SHUFFLE(1, 1, 1, 1));
    __m256 _tmp_2 = _mm256_shuffle_epi32(_mm256_castps_si256(pv), _MM_SHUFFLE(2, 2, 2, 2));
    __m256 _tmp_3 = _mm256_shuffle_epi32(_mm256_castps_si256(pv), _MM_SHUFFLE(3, 3, 3, 3));

    __m256 _tmp_4 = _mm256_mul_ps(ROW_256(mat, 0), _mm256_castsi256_ps(_tmp_0));
    __m256 _tmp_5 = _mm256_mul_ps(ROW_256(mat, 0), _mm256_castsi256_ps(_tmp_1));
    __m256 _tmp_6 = _mm256_mul_ps(ROW_256(mat, 1), _mm256_castsi256_ps(_tmp_2));
    __m256 _tmp_7 = _mm256_mul_ps(ROW_256(mat, 1), _mm256_castsi256_ps(_tmp_3));

    return _mm256_add_ps(_mm256_add_ps(_tmp_4, _tmp_5), _mm256_add_ps(_tmp_6, _tmp_7));
}
#endif

#if defined(__AVX512F__)
ALWAYS_INLINE static __m512 mat4x4_mulv4(const Mat4x4 *__restrict__ mat, __m512 pv) {
    __m512 _tmp_0 = _mm512_shuffle_epi32(_mm512_castps_si512(pv), _MM_SHUFFLE(0, 0, 0, 0));
    __m512 _tmp_1 = _mm512_shuffle_epi32(_mm512_castps_si512(pv), _MM_SHUFFLE(1, 1, 1, 1));
    __m512 _tmp_2 = _mm512_shuffle_epi32(_mm512_castps_si512(pv), _MM_SHUFFLE(2, 2, 2, 2));
    __m512 _tmp_3 = _mm512_shuffle_epi32(_mm512_castps_si512(pv), _MM_SHUFFLE(3, 3, 3, 3));

    __m512 _tmp_4 = _mm512_castps256_ps512(ROW_256(mat, 0));
    _tmp_4 = _mm512_inserti64x4(_tmp_4, _mm256_castps_si256(ROW_256(mat, 0)), 1);

    __m512 _tmp_5 = _mm512_castps256_ps512(ROW_256(mat, 1));
    _tmp_5 = _mm512_inserti64x4(_tmp_5, _mm256_castps_si256(ROW_256(mat, 1)), 1);

    __m512 _tmp_6 = _mm512_mul_ps(_mm512_castsi512_ps(_tmp_4), _mm512_castsi512_ps(_tmp_0));
    __m512 _tmp_7 = _mm512_mul_ps(_mm512_castsi512_ps(_tmp_4), _mm512_castsi512_ps(_tmp_1));
    __m512 _tmp_8 = _mm512_mul_ps(_mm512_castsi512_ps(_tmp_5), _mm512_castsi512_ps(_tmp_2));
    __m512 _tmp_9 = _mm512_mul_ps(_mm512_castsi512_ps(_tmp_5), _mm512_castsi512_ps(_tmp_3));

    return _mm512_add_ps(_mm512_add_ps(_tmp_6, _tmp_7), _mm512_add_ps(_tmp_8, _tmp_9));
}
#endif

ALWAYS_INLINE static void mat4x4_mulm(
        const Mat4x4 *__restrict__ a, const Mat4x4 *__restrict__ b, Mat4x4 *__restrict__ c) {

    // 4x4 matrices are stored in row-major order rather than column-major to speed up
    // 4x4 matrix vector and matrix multiplication
    // this implies that b itself doesn't need to be transposed to linearize the multiplication
#if defined(__AVX512F__)
    ROW_512(c, 0) = mat4x4_mulv4(a, ROW_512(b, 0));

#elif defined(__AVX2__)
    ROW_256(c, 0) = mat4x4_mulv2(a, ROW_256(b, 0));
    ROW_256(c, 1) = mat4x4_mulv2(a, ROW_256(b, 1));

#else
    ROW_128(c, 0) = mat4x4_mulv(a, *((vec4f *) b->val + 0)).vec;
    ROW_128(c, 1) = mat4x4_mulv(a, *((vec4f *) b->val + 1)).vec;
    ROW_128(c, 2) = mat4x4_mulv(a, *((vec4f *) b->val + 2)).vec;
    ROW_128(c, 3) = mat4x4_mulv(a, *((vec4f *) b->val + 3)).vec;

#endif
}

ALWAYS_INLINE static Mat4x4 mat4x4_from_rvec(const vec3f v[4]) {

    // 4x4 matrices are stored in row-major order rather than column-major to speed up
    // 4x4 matrix vector and matrix multiplication
    return (Mat4x4) { .rows = { v[0].vec, v[1].vec, v[2].vec, v[3].vec } };
}

ALWAYS_INLINE static Mat4x4 mat4x4_from_cvec(const vec3f v[4]) {

    // transpose needed to ensure row-major order
    Mat4x4 mat = { .rows = { v[0].vec, v[1].vec, v[2].vec, v[3].vec } };
    Mat4x4 dst;

    mat4x4_transpose(&mat, &dst);
    return dst;
}

ALWAYS_INLINE static Mat2x2 mat4x4_to_mat2x2(const Mat4x4 *__restrict__ src) {
    Mat2x2 mat = { .row = _mm_movelh_ps(ROW_128(src, 0), ROW_128(src, 1)) };
    Mat2x2 dst;

    // 2x2 matrices are stored in column-major order in contrast to
    // the row-major representation of 4x4 matrices
    mat2x2_transpose(&mat, &dst);
    return dst;
}

ALWAYS_INLINE static Mat4x4 mat2x2_to_mat4x4(const Mat2x2 *__restrict__ src) {
    /* TODO */
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
