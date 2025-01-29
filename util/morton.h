//
// Created by Luis Ruisinger on 28.01.25.
//

#ifndef CPU_RAYTRACING_MORTON_H
#define CPU_RAYTRACING_MORTON_H

#include "defines.h"
#include "../bvh/aabb.h"

ALWAYS_INLINE static inline u64 expand_64(u64 x) {
    x = (x | (x << 16)) & 0x0000003F0000FFFFull;
    x = (x | (x << 16)) & 0x003F0000FF0000FFull;
    x = (x | (x <<  8)) & 0x300F00F00F00F00Full;
    x = (x | (x <<  4)) & 0x30C30C30C30C30C3ull;
    x = (x | (x <<  2)) & 0x9249249249249249ull;
    return x;
}

ALWAYS_INLINE static inline __m256 _mm256_expand_64_epi64(__m256 _x) {
    union {
        u64 u[5]; i64 i[5];
    } cast;

    cast.u[0] = 0x0000003F0000FFFFULL;
    cast.u[1] = 0x003F0000FF0000FFULL;
    cast.u[2] = 0x300F00F00F00F00FULL;
    cast.u[3] = 0x30C30C30C30C30C3ULL;
    cast.u[4] = 0x9249249249249249ULL;

    _x = _mm256_and_si256(_mm256_or_si256(_x, _mm256_slli_epi64(_x, 16)), _mm256_set1_epi64x(cast.i[0]));
    _x = _mm256_and_si256(_mm256_or_si256(_x, _mm256_slli_epi64(_x, 16)), _mm256_set1_epi64x(cast.i[1]));
    _x = _mm256_and_si256(_mm256_or_si256(_x, _mm256_slli_epi64(_x,  8)), _mm256_set1_epi64x(cast.i[2]));
    _x = _mm256_and_si256(_mm256_or_si256(_x, _mm256_slli_epi64(_x,  4)), _mm256_set1_epi64x(cast.i[3]));
    _x = _mm256_and_si256(_mm256_or_si256(_x, _mm256_slli_epi64(_x,  2)), _mm256_set1_epi64x(cast.i[4]));

    return _x;
}


ALWAYS_INLINE static inline u32 expand_32(u64 x) {
    x = (x | (x << 16)) & 0x070000FF;
    x = (x | (x <<  8)) & 0x0700F00F;
    x = (x | (x <<  4)) & 0x430C30C3;
    x = (x | (x <<  2)) & 0x49249249;

    return x;
}

ALWAYS_INLINE static inline __m256 _mm256_expand_32_epi64(__m256 _x) {
    union {
        u64 u[4]; i64 i[4];
    } cast;

    cast.u[0] = 0x070000FFULL;
    cast.u[1] = 0x0700F00FULL;
    cast.u[2] = 0x430C30C3ULL;
    cast.u[3] = 0x49249249ULL;

    _x = _mm256_and_si256(_mm256_or_si256(_x, _mm256_slli_epi64(_x, 16)), _mm256_set1_epi64x(cast.i[0]));
    _x = _mm256_and_si256(_mm256_or_si256(_x, _mm256_slli_epi64(_x,  8)), _mm256_set1_epi64x(cast.i[1]));
    _x = _mm256_and_si256(_mm256_or_si256(_x, _mm256_slli_epi64(_x,  4)), _mm256_set1_epi64x(cast.i[2]));
    _x = _mm256_and_si256(_mm256_or_si256(_x, _mm256_slli_epi64(_x,  2)), _mm256_set1_epi64x(cast.i[3]));

    return _x;
}

ALWAYS_INLINE static inline void encode(u32 x, u32 y, u32 z, u32 morton[3]) {
    const u32 last_21_bits = (1ULL << 21) - 1;
    const u32 last_22_bits = (1ULL << 22) - 1;
    const u32 last_32_bits = (1ULL << 32) - 1;

    const u32 upper_11_bits = ~last_21_bits;
    const u32 upper_10_bits = ~last_22_bits;

    u64 x12 = expand_64(x & last_22_bits);
    u64 y12 = expand_64(y & last_21_bits);
    u64 z12 = expand_64(z & last_21_bits);

    u32 x0  = expand_32((x & upper_10_bits) >> 22);
    u32 y0  = expand_32((y & upper_11_bits) >> 21);
    u32 z0  = expand_32((z & upper_11_bits) >> 21);

    u64 morton_12 = (x12 << 0) | (y12 << 1) | (z12 << 2);
    u32 morton_0  = (x0  << 0) | (y0  << 1) | (z0  << 2);

    morton[0] = morton_0;
    morton[1] = (u32) (morton_12 >> 32);
    morton[2] = (u32) (morton_12 >> 0);
}

ALWAYS_INLINE static inline void _mm_encode(__m128 _x, u32 morton[3]) {
    union {
        u64 u[4]; i64 i[4];
    } cast;

    cast.u[0] =  (1ULL << 21) - 1;
    cast.u[1] =  (1ULL << 22) - 1;
    cast.u[2] = ~(1ULL << 21) - 1;
    cast.u[3] = ~(1ULL << 22) - 1;

    __m256 _tmp_0 = _mm256_cvtepi32_epi64(_x);
    __m256 _tmp_1 = _mm256_set_epi64x(0x0, cast.i[0], cast.i[0], cast.i[1]);
    __m256 _tmp_2 = _mm256_and_si256(_tmp_0, _tmp_1);
    _tmp_2 = _mm256_expand_64_epi64(_tmp_2);
    _tmp_2 = _mm256_srlv_epi64(_tmp_2, _mm256_set_epi64x(0, 0, 1, 2));

    __m256 _tmp_3 = _mm256_set_epi64x(0x0, cast.i[2], cast.i[2], cast.i[3]);
    __m256 _tmp_4 = _mm256_and_si256(_tmp_0, _tmp_3);
    _tmp_4 = _mm256_srlv_epi64(_tmp_4, _mm256_set_epi64x(0x0, 21, 21, 22));
    _tmp_4 = _mm256_expand_32_epi64(_tmp_4);

    // and ensures the value does not overflow into 64 bit
    // UINT32_MAX can be used here because it can be represented by an i64
    _tmp_4 = _mm256_and_si256(_tmp_4, _mm256_set1_epi64x(UINT32_MAX));
    _tmp_4 = _mm256_srlv_epi64(_tmp_4, _mm256_set_epi64x(0, 0, 1, 2));

    // the following two expressions will be optimized by a reasonable good compiler (e.g. clang)
    u64 morton_12 =
            _mm256_extract_epi64(_tmp_2, 0) | _mm256_extract_epi64(_tmp_2, 1) |
            _mm256_extract_epi64(_tmp_2, 2) | _mm256_extract_epi64(_tmp_2, 3);

    u64 morton_0 =
            _mm256_extract_epi64(_tmp_4, 0) | _mm256_extract_epi64(_tmp_4, 1) |
            _mm256_extract_epi64(_tmp_4, 2) | _mm256_extract_epi64(_tmp_4, 3);

    morton[0] = morton_0;
    morton[1] = (u32) (morton_12 >> 32);
    morton[2] = (u32) (morton_12 >>  0);
}



/**
 * @brief Encoding of 3d vectors as morton codes (z-curve order).
 * @param v      Arbitrary vector residing inside/on the edge(s) the aabb.
 *               UB if the vector is actually outside of the given AABB.
 * @param aabb   AABB encompassing all input vectors.
 *               The AABB should stay the same for encoding multiple vectors of the same context,
 *               because its being used to normalize each vector into a discrete integer.
 * @param morton Decaying pointer to an array of 3 u32 forming a 96bit encoding for an
 *               arbitrary given 3d vector.
 */
ALWAYS_INLINE static inline void morton3d_f32(vec3f v, AABB *aabb, u32 morton[3]) {
    vec3f d = vec3_sub(aabb->max, aabb->min);
    f32 x_norm = (VEC3_GET(v, 0) - VEC3_GET(aabb->min, 0)) / VEC3_GET(d, 0);
    f32 y_norm = (VEC3_GET(v, 1) - VEC3_GET(aabb->min, 1)) / VEC3_GET(d, 1);
    f32 z_norm = (VEC3_GET(v, 2) - VEC3_GET(aabb->min, 2)) / VEC3_GET(d, 2);

    // 32 bit per coordinate -> 96 bit encode
    const u32 bit_mask = (1ULL << 32) - 1;
    u32 x_int = (u32) (x_norm * (f32) bit_mask);
    u32 y_int = (u32) (y_norm * (f32) bit_mask);
    u32 z_int = (u32) (z_norm * (f32) bit_mask);

    encode(x_int, y_int, y_int, morton);
}

ALWAYS_INLINE static inline void _mm_morton3d_f32(vec3f v, AABB *aabb, u32 morton[3]) {
    vec3f d = vec3_sub(aabb->max, aabb->min);
    __m128 _tmp_0 = _mm_div_ps(_mm_sub_ps(v.vec, aabb->min.vec), d.vec);

    // 32 bit per coordinate -> 96 bit encode
    const f32 bit_mask = (f32) ((1ULL << 32) - 1);
    __m128 _tmp_1 = _mm_mul_ps(_tmp_0, _mm_set1_ps(bit_mask));

    _mm_encode(_tmp_1, morton);
}

#endif //CPU_RAYTRACING_MORTON_H