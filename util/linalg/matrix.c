//
// Created by Luis Ruisinger on 06.01.25.
//

#include "matrix.h"

void mat2x2_inverse(const Mat2x2 *__restrict__ src, Mat2x2 *__restrict__ dst) {
    f32 s = 1.0F / ((src->val[0] * src->val[3]) - (src->val[1] * src->val[2]));

    // this is strictly speaking wrong because we store 2 rows inside of one __m128
    // compared to a 4x4 matrix where one __m128 is exactly one row
    ROW(dst, 0) = _mm_shuffle_ps(ROW(src, 0), ROW(src, 0), _MM_SHUFFLE(3, 1, 2, 0));
    ROW(dst, 0) = _mm_mul_ps(ROW(dst, 0), _mm_set_ps(1.0F, -1.0F, -1.0F, 1.0F));
    ROW(dst, 0) = _mm_mul_ps(ROW(dst, 0), _mm_set1_ps(s));
}

vec4f mat4x4_mulv(const Mat4x4 *__restrict__ mat, vec4f v) {
    __m128 _tmp_0 = _mm_mul_ps(ROW(mat, 0), v.vec);
    __m128 _tmp_1 = _mm_mul_ps(ROW(mat, 1), v.vec);
    __m128 _tmp_2 = _mm_mul_ps(ROW(mat, 2), v.vec);
    __m128 _tmp_3 = _mm_mul_ps(ROW(mat, 3), v.vec);

    return VEC4(_mm_hsum_ps(_tmp_0), _mm_hsum_ps(_tmp_1),
                _mm_hsum_ps(_tmp_2), _mm_hsum_ps(_tmp_3));
}

void mat4x4_muls(const Mat4x4 *mat, Mat4x4 *dst, f32 s) {
    __m128 _tmp_0 = _mm_set1_ps(s);

    ROW(dst, 0) = _mm_mul_ps(ROW(mat, 0), _tmp_0);
    ROW(dst, 1) = _mm_mul_ps(ROW(mat, 1), _tmp_0);
    ROW(dst, 2) = _mm_mul_ps(ROW(mat, 2), _tmp_0);
    ROW(dst, 3) = _mm_mul_ps(ROW(mat, 3), _tmp_0);
}

void mat4x4_transpose(const Mat4x4 *__restrict__ src, Mat4x4 *__restrict__ dst) {
    __m128 _tmp_0 = _mm_shuffle_ps(ROW(src, 0), ROW(src, 1), 0x44);
    __m128 _tmp_1 = _mm_shuffle_ps(ROW(src, 0), ROW(src, 1), 0xEE);
    __m128 _tmp_2 = _mm_shuffle_ps(ROW(src, 2), ROW(src, 3), 0x44);
    __m128 _tmp_3 = _mm_shuffle_ps(ROW(src, 2), ROW(src, 3), 0xEE);

    ROW(dst, 0) = _mm_shuffle_ps(_tmp_0, _tmp_2, 0x88);
    ROW(dst, 1) = _mm_shuffle_ps(_tmp_0, _tmp_2, 0xDD);
    ROW(dst, 2) = _mm_shuffle_ps(_tmp_1, _tmp_3, 0x88);
    ROW(dst, 3) = _mm_shuffle_ps(_tmp_1, _tmp_3, 0xDD);
}

void mat4x4_mulm(
        const Mat4x4 *__restrict__ a, const Mat4x4 *__restrict__ b, Mat4x4 *__restrict__ c) {
    Mat4x4 t;
    mat4x4_transpose(b, &t);

    ROW(c, 0) = mat4x4_mulv(a, *((vec4f *) t.val + 0)).vec;
    ROW(c, 1) = mat4x4_mulv(a, *((vec4f *) t.val + 1)).vec;
    ROW(c, 2) = mat4x4_mulv(a, *((vec4f *) t.val + 2)).vec;
    ROW(c, 3) = mat4x4_mulv(a, *((vec4f *) t.val + 3)).vec;
}

/**
 * @brief Computes the inverse of an arbitrary 4x4 matrix.
 * @param src The source transformation matrix.
 * @param dst The destination transformation matrix where we want to store the inverse of src.
 */
void mat4x4_inverse(const Mat4x4 *__restrict__ src, Mat4x4 *__restrict__ dst) {
    Mat2x2 a = { .row = _mm_movelh_ps(ROW(src, 0), ROW(src, 1)) };
    Mat2x2 b = { .row = _mm_movehl_ps(ROW(src, 1), ROW(src, 0)) };
    Mat2x2 c = { .row = _mm_movelh_ps(ROW(src, 2), ROW(src, 3)) };
    Mat2x2 d = { .row = _mm_movehl_ps(ROW(src, 3), ROW(src, 2)) };

    // det
    __m128 _tmp_0 = _mm_shuffle_ps(ROW(src, 0), ROW(src, 2), _MM_SHUFFLE(2, 0, 2, 0));
    __m128 _tmp_1 = _mm_shuffle_ps(ROW(src, 1), ROW(src, 3), _MM_SHUFFLE(3, 1, 3, 1));
    __m128 _tmp_2 = _mm_shuffle_ps(ROW(src, 0), ROW(src, 2), _MM_SHUFFLE(3, 1, 3, 1));
    __m128 _tmp_3 = _mm_shuffle_ps(ROW(src, 1), ROW(src, 3), _MM_SHUFFLE(2, 0, 2, 0));
    __m128 _tmp_4 = _mm_sub_ps(_mm_mul_ps(_tmp_0, _tmp_1), _mm_mul_ps(_tmp_2, _tmp_3));

    __m128 _det_a = _mm_shuffle_ps(_tmp_4, _tmp_4, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 _det_b = _mm_shuffle_ps(_tmp_4, _tmp_4, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 _det_c = _mm_shuffle_ps(_tmp_4, _tmp_4, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 _det_d = _mm_shuffle_ps(_tmp_4, _tmp_4, _MM_SHUFFLE(3, 3, 3, 3));

    Mat2x2 d_c, a_b;
    mat2x2_adjmul(&d, &c, &d_c);
    mat2x2_adjmul(&a, &b, &a_b);

    Mat2x2 tmp_0, tmp_1;
    mat2x2_mulm(&b, &d_c, &tmp_0);
    mat2x2_mulm(&c, &a_b, &tmp_1);

    __m128 _x = _mm_sub_ps(_mm_mul_ps(_det_d, ROW(&a, 0)), ROW(&tmp_0, 0));
    __m128 _w = _mm_sub_ps(_mm_mul_ps(_det_a, ROW(&d, 0)), ROW(&tmp_1, 0));

    Mat2x2 tmp_2, tmp_3;
    mat2x2_muladj(&d, &a_b, &tmp_2);
    mat2x2_muladj(&a, &d_c, &tmp_3);

    __m128 _y = _mm_sub_ps(_mm_mul_ps(_det_b, ROW(&c, 0)), ROW(&tmp_2, 0));
    __m128 _z = _mm_sub_ps(_mm_mul_ps(_det_c, ROW(&b, 0)), ROW(&tmp_3, 0));

    __m128 _tr = _mm_shuffle_epi32(_mm_castps_si128(ROW(&d_c, 0)), _MM_SHUFFLE(3, 1, 2, 0));
    _tr = _mm_mul_ps(ROW(&a_b, 0), _mm_castsi128_ps(_tr));
    _tr = _mm_hadd_ps(_tr, _tr);
    _tr = _mm_hadd_ps(_tr, _tr);

    // matrix det
    __m128 _det = _mm_add_ps(_mm_mul_ps(_det_a, _det_d), _mm_mul_ps(_det_b, _det_c));
    _det = _mm_sub_ps(_det, _tr);
    _det = _mm_div_ps(_mm_setr_ps(1.0F, -1.0F, -1.0F, 1.0F), _det);

    _x = _mm_mul_ps(_x, _det);
    _y = _mm_mul_ps(_y, _det);
    _z = _mm_mul_ps(_z, _det);
    _w = _mm_mul_ps(_w, _det);

    ROW(dst, 0) = _mm_shuffle_ps(_x, _y, _MM_SHUFFLE(1, 3, 1, 3));
    ROW(dst, 1) = _mm_shuffle_ps(_x, _y, _MM_SHUFFLE(0, 2, 0, 2));
    ROW(dst, 2) = _mm_shuffle_ps(_z, _w, _MM_SHUFFLE(1, 3, 1, 3));
    ROW(dst, 3) = _mm_shuffle_ps(_z, _w, _MM_SHUFFLE(0, 2, 0, 2));
}

/**
 * @brief Computes the inverse of a transformation matrix which does not apply any scaling.
 * @param src The source transformation matrix.
 * @param dst The destination transformation matrix where we want to store the inverse of src.
 */
void mat4x4_inverse_tns(const Mat4x4 *__restrict__ src, Mat4x4 *__restrict__ dst) {
    Mat2x2 a = { .row = _mm_movelh_ps(ROW(src, 0), ROW(src, 1)) };
    Mat2x2 b = { .row = _mm_movehl_ps(ROW(src, 1), ROW(src, 0)) };

    ROW(dst, 0) = _mm_shuffle_ps(ROW(&a, 0), ROW(src, 2), _MM_SHUFFLE(3, 0, 2, 0));
    ROW(dst, 1) = _mm_shuffle_ps(ROW(&a, 0), ROW(src, 2), _MM_SHUFFLE(3, 1, 3, 1));
    ROW(dst, 2) = _mm_shuffle_ps(ROW(&b, 0), ROW(src, 2), _MM_SHUFFLE(3, 2, 2, 0));

    __m128 _tmp_0 = _mm_shuffle_epi32(_mm_castps_si128(ROW(src, 0)), _MM_SHUFFLE(0, 0, 0, 0));
    __m128 _tmp_1 = _mm_shuffle_epi32(_mm_castps_si128(ROW(src, 0)), _MM_SHUFFLE(1, 1, 1, 1));
    __m128 _tmp_2 = _mm_shuffle_epi32(_mm_castps_si128(ROW(src, 0)), _MM_SHUFFLE(2, 2, 2, 2));

    _tmp_0 = _mm_mul_ps(ROW(dst, 0), _mm_castsi128_ps(_tmp_0));
    _tmp_1 = _mm_mul_ps(ROW(dst, 1), _mm_castsi128_ps(_tmp_1));
    _tmp_2 = _mm_mul_ps(ROW(dst, 2), _mm_castsi128_ps(_tmp_2));

    ROW(dst, 3) = _mm_add_ps(_mm_add_ps(_tmp_0, _tmp_1), _tmp_2);
    ROW(dst, 3) = _mm_sub_ps(_mm_setr_ps(0.0F, 0.0F, 0.0F, 1.0F), ROW(dst, 3));
}

/**
 * @brief Computes the inverse of a general transformation matrix.
 * @param src The source transformation matrix.
 * @param dst The destination transformation matrix where we want to store the inverse of src.
 */
void mat4x4_inverse_t(const Mat4x4 *__restrict__ src, Mat4x4 *__restrict__ dst) {
    Mat2x2 a = { .row = _mm_movelh_ps(ROW(src, 0), ROW(src, 1)) };
    Mat2x2 b = { .row = _mm_movehl_ps(ROW(src, 1), ROW(src, 0)) };

    ROW(dst, 0) = _mm_shuffle_ps(ROW(&a, 0), ROW(src, 2), _MM_SHUFFLE(3, 0, 2, 0));
    ROW(dst, 1) = _mm_shuffle_ps(ROW(&a, 0), ROW(src, 2), _MM_SHUFFLE(3, 1, 3, 1));
    ROW(dst, 2) = _mm_shuffle_ps(ROW(&b, 0), ROW(src, 2), _MM_SHUFFLE(3, 2, 2, 0));

    __m128 _tmp_0 = _mm_mul_ps(ROW(dst, 0), ROW(dst, 0));
    _tmp_0 = _mm_add_ps(_tmp_0, _mm_mul_ps(ROW(dst, 1), ROW(dst, 1)));
    _tmp_0 = _mm_add_ps(_tmp_0, _mm_mul_ps(ROW(dst, 2), ROW(dst, 2)));

    __m128 _tmp_1 = _mm_set1_ps(1.0F);
    __m128 _tmp_2 = _mm_div_ps(_tmp_1, _tmp_0);
    _tmp_2 = _mm_blendv_ps(_tmp_2, _tmp_1, _mm_cmplt_ps(_tmp_0, _mm_set1_ps(1.0E-8F)));

    ROW(dst, 0) = _mm_mul_ps(ROW(dst, 0), _tmp_2);
    ROW(dst, 1) = _mm_mul_ps(ROW(dst, 1), _tmp_2);
    ROW(dst, 2) = _mm_mul_ps(ROW(dst, 2), _tmp_2);


    __m128 _tmp_3 = _mm_shuffle_epi32(_mm_castps_si128(ROW(src, 0)), _MM_SHUFFLE(0, 0, 0, 0));
    __m128 _tmp_4 = _mm_shuffle_epi32(_mm_castps_si128(ROW(src, 0)), _MM_SHUFFLE(1, 1, 1, 1));
    __m128 _tmp_5 = _mm_shuffle_epi32(_mm_castps_si128(ROW(src, 0)), _MM_SHUFFLE(2, 2, 2, 2));

    _tmp_3 = _mm_mul_ps(ROW(dst, 0), _mm_castsi128_ps(_tmp_3));
    _tmp_4 = _mm_mul_ps(ROW(dst, 1), _mm_castsi128_ps(_tmp_4));
    _tmp_5 = _mm_mul_ps(ROW(dst, 2), _mm_castsi128_ps(_tmp_5));

    ROW(dst, 3) = _mm_add_ps(_mm_add_ps(_tmp_3, _tmp_4), _tmp_5);
    ROW(dst, 3) = _mm_sub_ps(_mm_setr_ps(0.0F, 0.0F, 0.0F, 1.0F), ROW(dst, 3));
}





