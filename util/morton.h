//
// Created by Luis Ruisinger on 28.01.25.
//

#ifndef CPU_RAYTRACING_MORTON_H
#define CPU_RAYTRACING_MORTON_H

#include "defines.h"
#include "../bvh/aabb.h"

#define LOAD_UNSIGNED_64(_x) \
    _mm256_set1_epi64x(*(i64 *) (_x))

ALWAYS_INLINE static inline u64 encode(u32 x) {
    u64 y = x;
    
    y = (y | y << 32) & 0x001F00000000FFFFULL; 
    y = (y | y << 16) & 0x001F0000FF0000FFULL; 
    y = (y | y <<  8) & 0x100F00F00F00F00FULL; 
    y = (y | y <<  4) & 0x10c30c30c30c30c3ULL; 
    y = (y | y <<  2) & 0x1249249249249249ULL;

    return y;
}

ALWAYS_INLINE static inline __m256 encode_avx2(__m128 _x) {
    const __m256 _tmp_0 = LOAD_UNSIGNED_64(0x001F00000000FFFFULL);
    const __m256 _tmp_1 = LOAD_UNSIGNED_64(0x001F0000FF0000FFULL);
    const __m256 _tmp_2 = LOAD_UNSIGNED_64(0x100F00F00F00F00FULL);
    const __m256 _tmp_3 = LOAD_UNSIGNED_64(0x10c30c30c30c30c3ULL);
    const __m256 _tmp_4 = LOAD_UNSIGNED_64(0x1249249249249249ULL);

    __m256 _y = _mm256_cvtepi32_epi64(_x);
    _y = _mm256_and_si256(_mm256_or_si256(_y, _mm256_slli_epi64(_y, 16)), _tmp_0);
    _y = _mm256_and_si256(_mm256_or_si256(_y, _mm256_slli_epi64(_y, 16)), _tmp_1);
    _y = _mm256_and_si256(_mm256_or_si256(_y, _mm256_slli_epi64(_y,  8)), _tmp_2);
    _y = _mm256_and_si256(_mm256_or_si256(_y, _mm256_slli_epi64(_y,  4)), _tmp_3);
    _y = _mm256_and_si256(_mm256_or_si256(_y, _mm256_slli_epi64(_y,  2)), _tmp_4);

    return _y;
}

/**
 * @brief  Encodes a single f32 into its morton code representation. 
 *         Utilizes just the lowest 21 bits. 
 *         Normalizes from [min, max] -> [0, 1] and then expands into a 21 bit u32 representation.
 * @param  x   f32 arbitrary value. 
 * @param  min interval lower limit.
 * @param  max interval upper limit.
 * @return 64 bit encoded morton code. 
 */
ALWAYS_INLINE static inline u64 morton_f32(f32 x, f32 min, f32 max) {
    
    // distributing over 21 bits to compress up to 3 dimensions into a single u64
    const u32 bit_mask = (1UL << 21) - 1;
    
    f32 x_norm = (x - min) / (max - min);
    u32 x_int = (u32) (x_norm * (f32) bit_mask);        
    return encode(x_int);
}

/**
 * @brief  Encodes a single u32 into its morton code representation. 
 *         Utilizes just the lowest 21 bits. 
 * @param  x u32 arbitrary value. 
 * @return 64 bit encoded morton code. 
 */
ALWAYS_INLINE static inline u64 morton_u32(u32 x) {
    
    // distributing over 21 bits to compress up to 3 dimensions into a single u64
    const u32 bit_mask = (1UL << 21) - 1;
    u32 x_int = x & bit_mask;
    return encode(x_int);
}

/**
 * @brief  Encoding of 3d vectors as morton codes (z-curve order).
 * @param  v    Arbitrary vector residing inside/on the edge(s) the aabb.
 *              UB if the vector is actually outside of the given AABB.
 * @param  aabb AABB encompassing all input vectors.
 *              The AABB should stay the same for encoding multiple vectors of the same context,
 *              because its being used to normalize each vector into a discrete integer.
 * @return 64 bit encoded morton code by interleaving the 3 f32 from the vector.
 */
ALWAYS_INLINE static inline u64 morton_vec(vec3f v, AABB *aabb) {
#if defined(__AVX2__)

    // distributing over 21 bits to compress up to 3 dimensions into a single u64
    const u32 bit_mask = (1UL << 21) - 1;

    // vec4 inlined functions have 0 overhead as we just ignore the w entry
    // they resolve to vector instructions 
    vec3f d = vec4_sub(aabb->max, aabb->min);
    d = vec4_div(vec4_sub(v, aabb->min), d);
    d = vec4_mul(d, VEC3((f32) bit_mask));

    __m256 _tmp_0 = encode_avx2(d.vec);
    return 
        (_mm256_extract_epi64(_tmp_0, 0) << 0) | 
        (_mm256_extract_epi64(_tmp_0, 1) << 1) | 
        (_mm256_extract_epi64(_tmp_0, 2) << 2);
#else
    vec3f d = vec3_sub(aabb->max, aabb->min);
    f32 x_norm = (VEC3_GET(v, 0) - VEC3_GET(aabb->min, 0)) / VEC3_GET(d, 0);
    f32 y_norm = (VEC3_GET(v, 1) - VEC3_GET(aabb->min, 1)) / VEC3_GET(d, 1);
    f32 z_norm = (VEC3_GET(v, 2) - VEC3_GET(aabb->min, 2)) / VEC3_GET(d, 2);

    // distributing over 21 bits to compress up to 3 dimensions into a single u64
    const u32 bit_mask = (1UL << 21) - 1;
    u32 x_int = (u32) (x_norm * (f32) bit_mask);
    u32 y_int = (u32) (y_norm * (f32) bit_mask);
    u32 z_int = (u32) (z_norm * (f32) bit_mask);

    return encode(x_int) | (encode(y_int) << 1) | (encode(z_int) << 2);
#endif
}

#undef LOAD_UNSIGNED_64
#endif //CPU_RAYTRACING_MORTON_H
