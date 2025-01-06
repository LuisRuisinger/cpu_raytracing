//
// Created by Luis Ruisinger on 06.01.25.
//

#include "matrix.h"

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

void mat4x4_transpose(const Mat4x4 *__restrict__ mat, Mat4x4 *__restrict__ dst) {
    __m128 _tmp_0 = _mm_shuffle_ps(ROW(mat, 0), ROW(mat, 1), 0x44);
    __m128 _tmp_1 = _mm_shuffle_ps(ROW(mat, 0), ROW(mat, 1), 0xEE);
    __m128 _tmp_2 = _mm_shuffle_ps(ROW(mat, 2), ROW(mat, 3), 0x44);
    __m128 _tmp_3 = _mm_shuffle_ps(ROW(mat, 2), ROW(mat, 3), 0xEE);

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

ALWAYS_INLINE __m128 mat2x2_mulm(__m128 _a, __m128 _b) {
    __m128 _tmp_0 = (__m128) _mm_shuffle_epi32(_mm_castps_si128(_b), _MM_SHUFFLE(3, 0, 3, 0));
    __m128 _tmp_1 = (__m128) _mm_shuffle_epi32(_mm_castps_si128(_a), _MM_SHUFFLE(2, 3, 0, 1));
    __m128 _tmp_2 = (__m128) _mm_shuffle_epi32(_mm_castps_si128(_b), _MM_SHUFFLE(1, 2, 1, 2));

    __m128 _tmp_3 = _mm_mul_ps(_a, _mm_castsi128_ps((__m128i) _tmp_0));
    __m128 _tmp_4 = _mm_mul_ps(
            _mm_castsi128_ps((__m128i) _tmp_1), _mm_castsi128_ps((__m128i) _tmp_2));

    return _mm_add_ps(_tmp_3, _tmp_4);
}

ALWAYS_INLINE __m128 mat2x2_adjmul(__m128 _a, __m128 _b) {
    __m128 _tmp_0 = (__m128) _mm_shuffle_epi32(_mm_castps_si128(_a), _MM_SHUFFLE(0, 3, 0, 3));
    __m128 _tmp_1 = (__m128) _mm_shuffle_epi32(_mm_castps_si128(_a), _MM_SHUFFLE(2, 2, 1, 1));
    __m128 _tmp_2 = (__m128) _mm_shuffle_epi32(_mm_castps_si128(_b), _MM_SHUFFLE(1, 0, 3, 2));

    __m128 _tmp_3 = _mm_mul_ps(_mm_castsi128_ps(_tmp_0), _b);
    __m128 _tmp_4 = _mm_mul_ps(
            _mm_castsi128_ps((__m128i) _tmp_1), _mm_castsi128_ps((__m128i) _tmp_2));

    return _mm_sub_ps(_tmp_3, _tmp_4);
}

ALWAYS_INLINE __m128 mat2x2_muladj(__m128 _a, __m128 _b) {
    __m128 _tmp_0 = (__m128) _mm_shuffle_epi32(_mm_castps_si128(_b), _MM_SHUFFLE(0, 3, 0, 3));
    __m128 _tmp_1 = (__m128) _mm_shuffle_epi32(_mm_castps_si128(_a), _MM_SHUFFLE(2, 3, 0, 1));
    __m128 _tmp_2 = (__m128) _mm_shuffle_epi32(_mm_castps_si128(_b), _MM_SHUFFLE(1, 2, 1, 2));

    __m128 _tmp_3 = _mm_mul_ps(_a, _mm_castsi128_ps(_tmp_0));
    __m128 _tmp_4 = _mm_mul_ps(
            _mm_castsi128_ps((__m128i) _tmp_1), _mm_castsi128_ps((__m128i) _tmp_2));

    return _mm_sub_ps(_tmp_3, _tmp_4);
}

void mat4x4_inverse(const Mat4x4 *__restrict__ mat, Mat4x4 *__restrict__ dst) {
    __m128 _a = _mm_movelh_ps(ROW(mat, 0), ROW(mat, 1));
    __m128 _b = _mm_movehl_ps(ROW(mat, 1), ROW(mat, 0));
    __m128 _c = _mm_movelh_ps(ROW(mat, 2), ROW(mat, 3));
    __m128 _d = _mm_movehl_ps(ROW(mat, 3), ROW(mat, 2));

    // det
    __m128 _tmp_0 = _mm_shuffle_ps(ROW(mat, 0), ROW(mat, 2), _MM_SHUFFLE(2, 0, 2, 0));
    __m128 _tmp_1 = _mm_shuffle_ps(ROW(mat, 1), ROW(mat, 3), _MM_SHUFFLE(3, 1, 3, 1));
    __m128 _tmp_2 = _mm_shuffle_ps(ROW(mat, 0), ROW(mat, 2), _MM_SHUFFLE(3, 1, 3, 1));
    __m128 _tmp_3 = _mm_shuffle_ps(ROW(mat, 1), ROW(mat, 3), _MM_SHUFFLE(2, 0, 2, 0));
    __m128 _tmp_4 = _mm_sub_ps(_mm_mul_ps(_tmp_0, _tmp_1), _mm_mul_ps(_tmp_2, _tmp_3));

    __m128 _det_a = _mm_shuffle_ps(_tmp_4, _tmp_4, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 _det_b = _mm_shuffle_ps(_tmp_4, _tmp_4, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 _det_c = _mm_shuffle_ps(_tmp_4, _tmp_4, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 _det_d = _mm_shuffle_ps(_tmp_4, _tmp_4, _MM_SHUFFLE(3, 3, 3, 3));

    __m128 _d_c = mat2x2_adjmul(_d, _c);
    __m128 _a_b = mat2x2_adjmul(_a, _b);
    __m128 _x = _mm_sub_ps(_mm_mul_ps(_det_d, _a), mat2x2_mulm(_b, _d_c));
    __m128 _w = _mm_sub_ps(_mm_mul_ps(_det_a, _d), mat2x2_mulm(_c, _a_b));
    __m128 _y = _mm_sub_ps(_mm_mul_ps(_det_b, _c), mat2x2_muladj(_d, _a_b));
    __m128 _z = _mm_sub_ps(_mm_mul_ps(_det_c, _b), mat2x2_muladj(_a, _d_c));

    __m128 _tr = (__m128) _mm_shuffle_epi32(_mm_castps_si128(_d_c), _MM_SHUFFLE(3, 1, 2, 0));
    _tr = _mm_mul_ps(_a_b, _mm_castsi128_ps((__m128i) _tr));

    /* TODO maybe change to hsum */
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





