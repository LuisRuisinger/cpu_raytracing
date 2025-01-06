//
// Created by Luis Ruisinger on 06.01.25.
//

#include "matrix.h"

vec4f matrix_4x4_mulv(const Matrix_4x4 *mat, vec4f v) {
    __m128 _tmp_0 = _mm_mul_ps(mat->rows[0], v.vec);
    __m128 _tmp_1 = _mm_mul_ps(mat->rows[1], v.vec);
    __m128 _tmp_2 = _mm_mul_ps(mat->rows[2], v.vec);
    __m128 _tmp_3 = _mm_mul_ps(mat->rows[3], v.vec);

    return VEC4(_mm_hsum_ps(_tmp_0), _mm_hsum_ps(_tmp_1),
                _mm_hsum_ps(_tmp_2), _mm_hsum_ps(_tmp_3));
}

Matrix_4x4 matrix_4x4_muls(const Matrix_4x4 *mat, f32 s) {
    __m128 _tmp_0 = _mm_set1_ps(s);

    return (Matrix_4x4) {
        .rows = {
                _mm_mul_ps(mat->rows[0], _tmp_0),
                _mm_mul_ps(mat->rows[1], _tmp_0),
                _mm_mul_ps(mat->rows[2], _tmp_0),
                _mm_mul_ps(mat->rows[3], _tmp_0),
        }
    };
}

Matrix_4x4 matrix_4x4_transpose(const Matrix_4x4 *mat) {
    __m128 _tmp_0 = _mm_shuffle_ps(mat->rows[0], mat->rows[1], 0x44);
    __m128 _tmp_1 = _mm_shuffle_ps(mat->rows[0], mat->rows[1], 0xEE);
    __m128 _tmp_2 = _mm_shuffle_ps(mat->rows[2], mat->rows[3], 0x44);
    __m128 _tmp_3 = _mm_shuffle_ps(mat->rows[2], mat->rows[3], 0xEE);

    return (Matrix_4x4) {
        .rows = {
                _mm_shuffle_ps(_tmp_0, _tmp_2, 0x88),
                _mm_shuffle_ps(_tmp_0, _tmp_2, 0xDD),
                _mm_shuffle_ps(_tmp_1, _tmp_3, 0x88),
                _mm_shuffle_ps(_tmp_1, _tmp_3, 0xDD)
        }
    };
}

Matrix_4x4 matrix_4x4_mulm(const Matrix_4x4 *a, const Matrix_4x4 *b) {
    Matrix_4x4 c = matrix_4x4_transpose(b);

    return (Matrix_4x4) {
        .rows = {
                matrix_4x4_mulv(a, *(vec4f *) &c.rows[0]).vec,
                matrix_4x4_mulv(a, *(vec4f *) &c.rows[1]).vec,
                matrix_4x4_mulv(a, *(vec4f *) &c.rows[2]).vec,
                matrix_4x4_mulv(a, *(vec4f *) &c.rows[3]).vec
        }
    };
}

