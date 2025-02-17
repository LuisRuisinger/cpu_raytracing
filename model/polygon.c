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
    vec3f tmp = vec3_add((triangle)->point[0], (triangle)->point[1]);
    tmp = vec3_add(tmp, (triangle)->point[2]);

    triangle->centroid = vec3_muls(tmp, 0.3333F);
}

vec3f triangle_calc_barycentric_coords(Triangle *tri, vec3f hit) {
    vec3f v_0 = vec3_sub(tri->point[1], tri->point[0]);
    vec3f v_1 = vec3_sub(tri->point[2], tri->point[0]);
    vec3f v_2 = vec3_sub(hit, tri->point[0]);

    f32 d_20 = vec3_dot(v_2, v_0);
    f32 d_21 = vec3_dot(v_2, v_1);

    f32 v = (tri->self_dot[1] * d_20 - tri->dot[0] * d_21) / tri->denom;
    f32 w = (tri->self_dot[0] * d_21 - tri->dot[0] * d_20) / tri->denom;
    f32 u = 1.0F - v - w;

    return VEC3(u, v, w);
}
