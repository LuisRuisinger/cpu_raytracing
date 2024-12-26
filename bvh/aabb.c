//
// Created by Luis Ruisinger on 26.11.24.
//

#include "aabb.h"

void scale_vec(AABB *aabb, vec3f vec) {
    vec3f tmp = sub_vec(aabb->max, aabb->min);
    aabb->max = mul_vec(tmp, vec);
}

void scale_scalar(AABB *aabb, f32 scalar) {
    vec3f vec = VEC3(scalar, scalar, scalar);
    scale_vec(aabb, vec);
}

void scale_center_vec(AABB *aabb, vec3f vec) {
    vec3f tmp_0 = add_vec(aabb->max, aabb->min);
    tmp_0 = mul_scalar(tmp_0, 0.5F);

    vec3f tmp_1 = sub_vec(aabb->max, aabb->min);
    tmp_1 = mul_scalar(tmp_1, 0.5F);
    tmp_1 = mul_vec(tmp_1, vec);

    aabb->min = sub_vec(tmp_0, tmp_1);
    aabb->max = add_vec(tmp_0, tmp_1);
}

void scale_center_scalar(AABB *aabb, f32 scalar) {
    vec3f vec = VEC3(scalar, scalar, scalar);
    scale_center_vec(aabb, vec);
}

void translate_vec(AABB *aabb, vec3f vec) {
    aabb->min = add_vec(aabb->min, vec);
    aabb->max = add_vec(aabb->max, vec);
}

void translate_scalar(AABB *aabb, f32 scalar) {
    vec3f vec = VEC3(scalar);
    translate_vec(aabb, vec);
}

bool single_aabb_aabb_intersection(const AABB *a, const AABB *b) {
    __m128 tmp_0 = _mm_cmple_ps(a->min.vec, a->max.vec);
    __m128 tmp_1 = _mm_cmpge_ps(a->max.vec, b->min.vec);
    tmp_1 = _mm_and_si128(tmp_0, tmp_1);

    return (bool) _mm_test_all_ones(tmp_1);
}

bool single_ray_aabb_intersection(const Ray *ray, const AABB *aabb) {
    const __m128 t_min_vec = _mm_set1_ps(0.0F);
    const __m128 t_max_vec = _mm_set1_ps(INFINITY);

    __m128 tmp_0 = _mm_sub_ps(aabb->min.vec, _mm256_castps256_ps128(ray->org));
    tmp_0 = _mm_mul_ps(tmp_0, _mm256_castps256_ps128(ray->inv_dir));

    __m128 tmp_1 = _mm_sub_ps(aabb->max.vec, _mm256_castps256_ps128(ray->org));
    tmp_1 = _mm_mul_ps(tmp_1, _mm256_castps256_ps128(ray->inv_dir));

    __m128 tmp_0_max = _mm_max_ps(tmp_0, t_min_vec);
    __m128 tmp_0_min = _mm_min_ps(tmp_0, t_max_vec);

    __m128 tmp_1_max = _mm_max_ps(tmp_1, t_min_vec);
    __m128 tmp_1_min = _mm_min_ps(tmp_1, t_max_vec);

    __m128 tmp_2 = _mm_min_ps(tmp_0_max, tmp_1_max);
    __m128 tmp_3 = _mm_max_ps(tmp_0_min, tmp_1_min);

    __m128 result = _mm_cmple_ps(tmp_2, tmp_3);

    return (bool) _mm_test_all_ones(result);
}

__m128 single_ray_aabb_intersection_distance(const Ray *ray, const AABB *aabb) {
    const __m128 t_min_vec = _mm_set1_ps(0.0F);
    const __m128 t_max_vec = _mm_set1_ps(INFINITY);

    __m128 tmp_0 = _mm_sub_ps(aabb->min.vec, _mm256_castps256_ps128(ray->org));
    tmp_0 = _mm_mul_ps(tmp_0, _mm256_castps256_ps128(ray->inv_dir));

    __m128 tmp_1 = _mm_sub_ps(aabb->max.vec, _mm256_castps256_ps128(ray->org));
    tmp_1 = _mm_mul_ps(tmp_1, _mm256_castps256_ps128(ray->inv_dir));

    __m128 tmp_0_max = _mm_max_ps(tmp_0, t_min_vec);
    __m128 tmp_0_min = _mm_min_ps(tmp_0, t_max_vec);

    __m128 tmp_1_max = _mm_max_ps(tmp_1, t_min_vec);
    __m128 tmp_1_min = _mm_min_ps(tmp_1, t_max_vec);

    __m128 tmp_2 = _mm_min_ps(tmp_0_max, tmp_1_max);
    __m128 tmp_3 = _mm_max_ps(tmp_0_min, tmp_1_min);
    __m128 tmp_4 = _mm_cmp_ps(tmp_2, tmp_3, _CMP_LE_OQ);

    return _mm_blendv_ps(t_max_vec, tmp_2, tmp_4);
}

__m256 dual_ray_aabb_intersection(const Ray *ray, const AABB aabb[2]) {
    const __m256 t_min_vec = _mm256_set1_ps(0.0F);
    const __m256 t_max_vec = _mm256_set1_ps(INFINITY);

    __m256 tmp_0 = _mm256_set_m128(aabb[0].min.vec, aabb[1].min.vec);
    tmp_0 = _mm256_sub_ps(tmp_0, ray->org);
    tmp_0 = _mm256_mul_ps(tmp_0, ray->inv_dir);

    __m256 tmp_1 = _mm256_set_m128(aabb[0].max.vec, aabb[1].max.vec);
    tmp_1 = _mm256_sub_ps(tmp_1, ray->org);
    tmp_1 = _mm256_mul_ps(tmp_1, ray->inv_dir);

    __m256 tmp_2 = _mm256_max_ps(tmp_0, t_min_vec);
    __m256 tmp_3 = _mm256_min_ps(tmp_0, t_max_vec);
    __m256 tmp_4 = _mm256_max_ps(tmp_1, t_min_vec);
    __m256 tmp_5 = _mm256_min_ps(tmp_1, t_max_vec);

    __m256 tmp_6 = _mm256_min_ps(tmp_2, tmp_4);
    __m256 tmp_7 = _mm256_max_ps(tmp_3, tmp_5);
    __m256 tmp_8 = _mm256_cmp_ps(tmp_6, tmp_7, _CMP_LE_OQ);

    return _mm256_blendv_ps(t_max_vec, tmp_6, tmp_8);
}
