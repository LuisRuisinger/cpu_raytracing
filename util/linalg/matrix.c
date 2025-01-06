//
// Created by Luis Ruisinger on 06.01.25.
//

#include "matrix.h"

vec4f matrix_4x4_mulv(const Mat4x4 *__restrict__ mat, vec4f v) {
    __m128 _tmp_0 = _mm_mul_ps(*((__m128*) &mat->val + 0), v.vec);
    __m128 _tmp_1 = _mm_mul_ps(*((__m128*) &mat->val + 1), v.vec);
    __m128 _tmp_2 = _mm_mul_ps(*((__m128*) &mat->val + 2), v.vec);
    __m128 _tmp_3 = _mm_mul_ps(*((__m128*) &mat->val + 3), v.vec);

    return VEC4(_mm_hsum_ps(_tmp_0), _mm_hsum_ps(_tmp_1),
                _mm_hsum_ps(_tmp_2), _mm_hsum_ps(_tmp_3));
}

void matrix_4x4_muls(const Mat4x4 *mat, Mat4x4 *dst, f32 s) {
    __m128 _tmp_0 = _mm_set1_ps(s);

    *((__m128*) &dst->val + 0) = _mm_mul_ps(*((__m128*) &mat->val + 0), _tmp_0);
    *((__m128*) &dst->val + 1) = _mm_mul_ps(*((__m128*) &mat->val + 1), _tmp_0);
    *((__m128*) &dst->val + 2) = _mm_mul_ps(*((__m128*) &mat->val + 2), _tmp_0);
    *((__m128*) &dst->val + 3) = _mm_mul_ps(*((__m128*) &mat->val + 3), _tmp_0);
}

void matrix_4x4_transpose(const Mat4x4 *__restrict__ mat, Mat4x4 *__restrict__ dst) {
    __m128 _tmp_0 = _mm_shuffle_ps(*((__m128*) &mat->val + 0), *((__m128*) &mat->val + 1), 0x44);
    __m128 _tmp_1 = _mm_shuffle_ps(*((__m128*) &mat->val + 0), *((__m128*) &mat->val + 1), 0xEE);
    __m128 _tmp_2 = _mm_shuffle_ps(*((__m128*) &mat->val + 2), *((__m128*) &mat->val + 3), 0x44);
    __m128 _tmp_3 = _mm_shuffle_ps(*((__m128*) &mat->val + 2), *((__m128*) &mat->val + 3), 0xEE);

    *((__m128*) &dst->val + 0) = _mm_shuffle_ps(_tmp_0, _tmp_2, 0x88);
    *((__m128*) &dst->val + 0) = _mm_shuffle_ps(_tmp_0, _tmp_2, 0xDD);
    *((__m128*) &dst->val + 0) = _mm_shuffle_ps(_tmp_1, _tmp_3, 0x88);
    *((__m128*) &dst->val + 0) = _mm_shuffle_ps(_tmp_1, _tmp_3, 0xDD);
}

void matrix_4x4_mulm(
        const Mat4x4 *__restrict__ a, const Mat4x4 *__restrict__ b, Mat4x4 *__restrict__ c) {
    Mat4x4 t;
    matrix_4x4_transpose(b, &t);

    *((__m128*) &c->val + 0) = matrix_4x4_mulv(a, *((vec4f *) t.val + 0)).vec;
    *((__m128*) &c->val + 1) = matrix_4x4_mulv(a, *((vec4f *) t.val + 1)).vec;
    *((__m128*) &c->val + 2) = matrix_4x4_mulv(a, *((vec4f *) t.val + 2)).vec;
    *((__m128*) &c->val + 3) = matrix_4x4_mulv(a, *((vec4f *) t.val + 3)).vec;
}

