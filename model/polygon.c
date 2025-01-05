//
// Created by Luis Ruisinger on 26.11.24.
//

#include <float.h>

#include "polygon.h"

static ALWAYS_INLINE __m256 calculate_edges_vec(const Triangle *triangle) {
    /*
    __m256 tmp_0 = _mm256_load_ps((f32 *) &triangle->point_b);
    __m256 tmp_1 = _mm256_broadcast_ps((const __m128 *) &triangle->point_a);
    return _mm256_sub_ps(tmp_0, tmp_1);
    */
}

void eval_surface_normal(Triangle *triangle) {
    __m256 edges_vec = calculate_edges_vec(triangle);

    // TODO: maybe swap around - im not quite sure about the order in intrinsic registers
    __m128 tmp_0 = _mm256_castps256_ps128(edges_vec);
    __m128 tmp_1 = _mm256_extractf128_ps(edges_vec, 1);

    __m128 tmp_2 = _mm_shuffle_ps(tmp_0, tmp_0, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 tmp_3 = _mm_shuffle_ps(tmp_1, tmp_1, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 tmp_4 = _mm_mul_ps(tmp_2, tmp_1);
    tmp_4 = _mm_shuffle_ps(tmp_4, tmp_4, _MM_SHUFFLE(3, 0, 2, 1));
    tmp_4 = _mm_fmsub_ps(tmp_2, tmp_3, tmp_4);

    _mm_store_ps((f32 *) &triangle->normal, tmp_4);
}

void eval_centroid(Triangle *triangle) {
    vec3f tmp = add_vec((triangle)->point[0], (triangle)->point[1]);
    tmp = add_vec(tmp, (triangle)->point[2]);

    triangle->centroid = mul_scalar(tmp, 0.3333F);
}
