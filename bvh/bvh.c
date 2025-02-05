//
// Created by Luis Ruisinger on 26.11.24.
//

#include <assert.h>

#include "bvh.h"
#include "../global.h"
#include "../model/obj_parser.h"
#include "../util/morton.h"

#define DEPTH 16
#define NO_INTERSECTION INFINITY

/**
 * @brief  Calculates the distance between the ray origin and possible intersection
 *         with 4/8/16 (depends on supported instruction set) packed aabbs in 3d space.
 *         In case no hit occurs for one particular aabb it's result
 *         would fall back to NO_INTERSECTION.
 * @param  ray  The ray containing origin, a normalized direction and
 *              a precomputed normalized vec3_inverse direction.
 * @param  node Packed SOA of 8 aabbs.
 * @return __m256 packed result of 8 computed possible intersections.
 */
#if defined(__AVX512F__)
ALWAYS_INLINE static inline __m512 ray_node_intersection(
        const BVH_Ray *restrict ray, const BVH_Node *restrict node) {
    __m512 _t_min = _mm512_set1_ps(0.0F);
    __m512 _t_max = _mm512_set1_ps(INFINITY);

    for (usize i = 0; i < DIMENSIONS; ++i) {
        __m512 _tmp_0 = _mm512_sub_ps(node->aabbs._min[i], ray->_org[i]);
        __m512 _t1 = _mm512_mul_ps(_tmp_0, ray->_inv_dir[i]);

        __m512 _tmp_1 = _mm512_sub_ps(node->aabbs._max[i], ray->_org[i]);
        __m512 _t2 = _mm512_mul_ps(_tmp_1, ray->_inv_dir[i]);

        _t_min = _mm512_min_ps(_mm512_max_ps(_t1, _t_min), _mm512_max_ps(_t2, _t_min));
        _t_max = _mm512_max_ps(_mm512_min_ps(_t1, _t_max), _mm512_min_ps(_t2, _t_max));
    }

    __mmask16 mask = _mm512_cmp_ps_mask(_t_min, _t_max, _CMP_LE_OQ);
    return _mm512_mask_blend_ps(mask, _t_min, _mm512_set1_ps(NO_INTERSECTION));
}

#elif defined(__AVX2__)
ALWAYS_INLINE static inline __m256 ray_node_intersection(
        const BVH_Ray *restrict ray, const BVH_Node *restrict node) {
    __m256 _t_min = _mm256_set1_ps(0.0F);
    __m256 _t_max = _mm256_set1_ps(INFINITY);

    for (usize i = 0; i < DIMENSIONS; ++i) {
        __m256 _tmp_0 = _mm256_sub_ps(node->aabbs._min[i], ray->_org[i]);
        __m256 _t1 = _mm256_mul_ps(_tmp_0, ray->_inv_dir[i]);

        __m256 _tmp_1 = _mm256_sub_ps(node->aabbs._max[i], ray->_org[i]);
        __m256 _t2 = _mm256_mul_ps(_tmp_1, ray->_inv_dir[i]);

        _t_min = _mm256_min_ps(_mm256_max_ps(_t1, _t_min), _mm256_max_ps(_t2, _t_min));
        _t_max = _mm256_max_ps(_mm256_min_ps(_t1, _t_max), _mm256_min_ps(_t2, _t_max));
    }

    __m256 _cmp = _mm256_cmp_ps(_t_min, _t_max, _CMP_LE_OQ);
    return _mm256_blendv_ps(_t_min, _mm256_set1_ps(NO_INTERSECTION), _cmp);
}

#else
ALWAYS_INLINE static inline __m128 ray_node_intersection(
        const BVH_Ray *restrict ray, const BVH_Node *restrict node) {
    __m128 _t_min = _mm_set1_ps(0.0F);
    __m128 _t_max = _mm_set1_ps(INFINITY);

    for (usize i = 0; i < DIMENSIONS; ++i) {
        __m128 _tmp_0 = _mm_sub_ps(node->aabbs._min[i], ray->_org[i]);
        __m128 _t1 = _mm_mul_ps(_tmp_0, ray->_inv_dir[i]);

        __m128 _tmp_1 = _mm_sub_ps(node->aabbs._max[i], ray->_org[i]);
        __m128 _t2 = _mm_mul_ps(_tmp_1, ray->_inv_dir[i]);

        _t_min = _mm_min_ps(_mm_max_ps(_t1, _t_min), _mm_max_ps(_t2, _t_min));
        _t_max = _mm_max_ps(_mm_min_ps(_t1, _t_max), _mm_min_ps(_t2, _t_max));
    }

    __m128 _cmp = _mm_cmp_ps(_t_min, _t_max, _CMP_LE_OQ);
    return _mm_blendv_ps(_t_min, _mm_set1_ps(NO_INTERSECTION), _cmp);
}

#endif





#if defined(__AVX512F__)
ALWAYS_INLINE static inline void avx_multi_cross(
        __m512 _res[DIMENSIONS], const __m512 _a[DIMENSIONS], const __m512 _b[DIMENSIONS]) {
    _res[0] = _mm512_fmsub_ps(_a[1], _b[2], _mm512_mul_ps(_b[1], _a[2]));
    _res[1] = _mm512_fmsub_ps(_a[2], _b[0], _mm512_mul_ps(_b[2], _a[0]));
    _res[2] = _mm512_fmsub_ps(_a[0], _b[1], _mm512_mul_ps(_b[0], _a[1]));
}

ALWAYS_INLINE static inline void avx_multi_sub(
        __m512 _res[DIMENSIONS], const __m512 _a[DIMENSIONS], const __m512 _b[DIMENSIONS]) {
    _res[0] = _mm512_sub_ps(_a[0], _b[0]);
    _res[1] = _mm512_sub_ps(_a[1], _b[1]);
    _res[2] = _mm512_sub_ps(_a[2], _b[2]);
}

ALWAYS_INLINE static inline __m512 avx_multi_dot(
        const __m512 _a[DIMENSIONS], const __m512 _b[DIMENSIONS]) {
    __m512 _tmp_0 = _mm512_mul_ps(_a[0], _b[0]);
    __m512 _tmp_1 = _mm512_fmadd_ps(_a[1], _b[1], _tmp_0);

    return _mm512_fmadd_ps(_a[2], _b[2], _tmp_1);
}

#elif defined(__AVX2__)
ALWAYS_INLINE static inline void avx_multi_cross(
        __m256 _res[DIMENSIONS], const __m256 _a[DIMENSIONS], const __m256 _b[DIMENSIONS]) {
    _res[0] = _mm256_fmsub_ps(_a[1], _b[2], _mm256_mul_ps(_b[1], _a[2]));
    _res[1] = _mm256_fmsub_ps(_a[2], _b[0], _mm256_mul_ps(_b[2], _a[0]));
    _res[2] = _mm256_fmsub_ps(_a[0], _b[1], _mm256_mul_ps(_b[0], _a[1]));
}

ALWAYS_INLINE static inline void avx_multi_sub(
        __m256 _res[DIMENSIONS], const __m256 _a[DIMENSIONS], const __m256 _b[DIMENSIONS]) {
    _res[0] = _mm256_sub_ps(_a[0], _b[0]);
    _res[1] = _mm256_sub_ps(_a[1], _b[1]);
    _res[2] = _mm256_sub_ps(_a[2], _b[2]);
}

ALWAYS_INLINE static inline __m256 avx_multi_dot(
        const __m256 _a[DIMENSIONS], const __m256 _b[DIMENSIONS]) {
    __m256 _tmp_0 = _mm256_mul_ps(_a[0], _b[0]);
    __m256 _tmp_1 = _mm256_fmadd_ps(_a[1], _b[1], _tmp_0);

    return _mm256_fmadd_ps(_a[2], _b[2], _tmp_1);
}

#else
ALWAYS_INLINE static inline void avx_multi_cross(
        __m128 _res[DIMENSIONS], const __m128 _a[DIMENSIONS], const __m128 _b[DIMENSIONS]) {
    _res[0] = _mm_fmsub_ps(_a[1], _b[2], _mm_mul_ps(_b[1], _a[2]));
    _res[1] = _mm_fmsub_ps(_a[2], _b[0], _mm_mul_ps(_b[2], _a[0]));
    _res[2] = _mm_fmsub_ps(_a[0], _b[1], _mm_mul_ps(_b[0], _a[1]));
}

ALWAYS_INLINE static inline void avx_multi_sub(
        __m128 _res[DIMENSIONS], const __m128 _a[DIMENSIONS], const __m128 _b[DIMENSIONS]) {
    _res[0] = _mm_sub_ps(_a[0], _b[0]);
    _res[1] = _mm_sub_ps(_a[1], _b[1]);
    _res[2] = _mm_sub_ps(_a[2], _b[2]);
}

ALWAYS_INLINE static inline __m128 avx_multi_dot(
        const __m128 _a[DIMENSIONS], const __m128 _b[DIMENSIONS]) {
    __m128 _tmp_0 = _mm_mul_ps(_a[0], _b[0]);
    __m128 _tmp_1 = _mm_fmadd_ps(_a[1], _b[1], _tmp_0);

    return _mm_fmadd_ps(_a[2], _b[2], _tmp_1);
}

#endif





/**
 * @brief  Calculates the distance between the ray origin and possible intersection
 *         with 4/8/16 (depends on available instruction set) packed triangles in 3d space.
 *         In case no hit occurs for one particular triangle it's result
 *         would fall back to NO_INTERSECTION.
 * @param ray  The ray containing origin, a normalized direction and
 *             a precomputed normalized vec3_inverse direction.
 * @param  node Packed SOA of 4/8/16 triangles.
 * @return __m256 packed result of 4/8/16 computed possible intersections.
 */
#if defined(__AVX512F__)
__m512 ray_triangle_intersection(const BVH_Ray *restrict ray, const BVH_Node *restrict node) {
    __m512 _q[DIMENSIONS];
    __m512 _s[DIMENSIONS];
    __m512 _r[DIMENSIONS];

    avx_multi_cross(_q, ray->_dir, node->triangles._edge_1);

    __m512 _a = avx_multi_dot(node->triangles._edge_0, _q);
    __m512 _f = _mm512_div_ps(_mm512_set1_ps(1.0F), _a);
    avx_multi_sub(_s, ray->_org, node->triangles._point_a);

    __m512 _u = _mm512_mul_ps(_f, avx_multi_dot(_s, _q));
    avx_multi_cross(_r, _s, node->triangles._edge_0);

    __m512 _v = _mm512_mul_ps(_f, avx_multi_dot(ray->_dir, _r));
    __m512 _t = _mm512_mul_ps(_f, avx_multi_dot(node->triangles._edge_1, _r));

    // (_a > neg_eps) && (_a < pos_eps)
    __mmask16 mask_0 = _mm512_cmp_ps_mask(_a, _mm512_set1_ps(-1e-6F), _CMP_GT_OQ);
    __mmask16 mask_1 = _mm512_cmp_ps_mask(_a, _mm512_set1_ps(1e-6F), _CMP_LT_OQ);
    __mmask16 cnd_0 = mask_0 & mask_1;

    // _u < 0.0F
    __mmask16 cnd_1 = _mm512_cmp_ps_mask(_u, _mm512_set1_ps(0.0F), _CMP_LT_OQ);

    // (_v < 0.0f) || (_u + _v > 1.0F)
    __mmask16 mask_2 = _mm512_cmp_ps_mask(_v, _mm512_set1_ps(0.0F), _CMP_LT_OQ);
    __mmask16 mask_3 = _mm512_cmp_ps_mask(_mm512_add_ps(_u, _v), _mm512_set1_ps(1.0F), _CMP_GT_OQ);
    __mmask16 cnd_2 = mask_2 | mask_3;

    // t < 0.0F
    __mmask16 cnd_3 = _mm512_cmp_ps_mask(_t, _mm512_set1_ps(0.0F), _CMP_LT_OQ);

    // combined result
    __mmask16 failed = cnd_0 | cnd_1 | cnd_2 | cnd_3;
    return _mm512_mask_blend_ps(failed, _t, _mm512_set1_ps(NO_INTERSECTION));
}

#elif defined(__AVX2__)
__m256 ray_triangle_intersection(const BVH_Ray *restrict ray, const BVH_Node *restrict node) {
    __m256 _q[DIMENSIONS];
    __m256 _s[DIMENSIONS];
    __m256 _r[DIMENSIONS];

    avx_multi_cross(_q, ray->_dir, node->triangles._edge_1);

    __m256 _a = avx_multi_dot(node->triangles._edge_0, _q);
    __m256 _f = _mm256_div_ps(_mm256_set1_ps(1.0F), _a);
    avx_multi_sub(_s, ray->_org, node->triangles._point_a);

    __m256 _u = _mm256_mul_ps(_f, avx_multi_dot(_s, _q));
    avx_multi_cross(_r, _s, node->triangles._edge_0);

    __m256 _v = _mm256_mul_ps(_f, avx_multi_dot(ray->_dir, _r));
    __m256 _t = _mm256_mul_ps(_f, avx_multi_dot(node->triangles._edge_1, _r));

    // (_a > neg_eps) && (_a < pos_eps)
    __m256 _tmp_0 = _mm256_cmp_ps(_a, _mm256_set1_ps(-1e-6F), _CMP_GT_OQ);
    __m256 _tmp_1 = _mm256_cmp_ps(_a, _mm256_set1_ps(1e-6F), _CMP_LT_OQ);
    __m256 _cnd_0 = _mm256_and_ps(_tmp_0, _tmp_1);

    // _u < 0.0F
    __m256 _cnd_1 = _mm256_cmp_ps(_u, _mm256_set1_ps(0.0F), _CMP_LT_OQ);

    // (_v < 0.0f) || (_u + _v > 1.0F)
    __m256 _tmp_2 = _mm256_cmp_ps(_v, _mm256_set1_ps(0.0F), _CMP_LT_OQ);
    __m256 _tmp_3 = _mm256_cmp_ps(_mm256_add_ps(_u, _v), _mm256_set1_ps(1.0F), _CMP_GT_OQ);
    __m256 _cnd_2 = _mm256_or_ps(_tmp_2, _tmp_3);

    // t < 0.0F
    __m256 _cnd_3 = _mm256_cmp_ps(_t, _mm256_set1_ps(0.0F), _CMP_LT_OQ);

    // combined result
    __m256 _failed = _mm256_or_ps(_mm256_or_ps(_cnd_0, _cnd_1), _mm256_or_ps(_cnd_2, _cnd_3));
    return _mm256_blendv_ps(_t, _mm256_set1_ps(NO_INTERSECTION), _failed);
}

#else
__m128 ray_triangle_intersection(const BVH_Ray *restrict ray, const BVH_Node *restrict node) {
    __m128 _q[DIMENSIONS];
    __m128 _s[DIMENSIONS];
    __m128 _r[DIMENSIONS];

    avx_multi_cross(_q, ray->_dir, node->triangles._edge_1);

    __m128 _a = avx_multi_dot(node->triangles._edge_0, _q);
    __m128 _f = _mm256_div_ps(_mm_set1_ps(1.0F), _a);
    avx_multi_sub(_s, ray->_org, node->triangles._point_a);

    __m128 _u = _mm_mul_ps(_f, avx_multi_dot(_s, _q));
    avx_multi_cross(_r, _s, node->triangles._edge_0);

    __m128 _v = _mm_mul_ps(_f, avx_multi_dot(ray->_dir, _r));
    __m128 _t = _mm_mul_ps(_f, avx_multi_dot(node->triangles._edge_1, _r));

    // (_a > neg_eps) && (_a < pos_eps)
    __m128 _tmp_0 = _mm_cmp_ps(_a, _mm_set1_ps(-1e-6F), _CMP_GT_OQ);
    __m128 _tmp_1 = _mm_cmp_ps(_a, _mm_set1_ps(1e-6F), _CMP_LT_OQ);
    __m128 _cnd_0 = _mm_and_ps(_tmp_0, _tmp_1);

    // _u < 0.0F
    __m128 _cnd_1 = _mm_cmp_ps(_u, _mm_set1_ps(0.0F), _CMP_LT_OQ);

    // (_v < 0.0f) || (_u + _v > 1.0F)
    __m128 _tmp_2 = _mm_cmp_ps(_v, _mm_set1_ps(0.0F), _CMP_LT_OQ);
    __m128 _tmp_3 = _mm_cmp_ps(_mm_add_ps(_u, _v), _mm_set1_ps(1.0F), _CMP_GT_OQ);
    __m128 _cnd_2 = _mm_or_ps(_tmp_2, _tmp_3);

    // t < 0.0F
    __m128 _cnd_3 = _mm_cmp_ps(_t, _mm_set1_ps(0.0F), _CMP_LT_OQ);

    // combined result
    __m128 _failed = _mm_or_ps(_mm_or_ps(_cnd_0, _cnd_1), _mm_or_ps(_cnd_2, _cnd_3));
    return _mm_blendv_ps(_t, _mm_set1_ps(NO_INTERSECTION), _failed);
}

#endif





/**
 * @brief  Broadcasts the smallest single-precision float in a __m128/__m256/__m512 vector.
 * @param  _a Single-precision float __m128/__m256/__m512 vector.
 * @return __m128/__m256/__m512 vector containing the smallest single-precision float
 */
#if defined(__AVX512F__)
ALWAYS_INLINE static inline __m512 _mm512_broadcastminss_ps(__m512 _a) {
    /* TODO */
}
#endif

#if defined(__AVX2__)
ALWAYS_INLINE static inline __m256 _mm256_broadcastminss_ps(__m256 _a) {
    __m256 _tmp_0 = _mm256_permute_ps(_a, 0b10110001);
    __m256 _tmp_1 = _mm256_min_ps(_a, _tmp_0);
    __m256 _tmp_2 = _mm256_permute_ps(_tmp_1, 0b01001110);
    __m256 _tmp_3 = _mm256_min_ps(_tmp_1, _tmp_2);
    __m256 _tmp_4 = _mm256_castpd_ps(_mm256_permute4x64_pd(_mm256_castps_pd(_tmp_3), 0b01001110));

    return  _mm256_min_ps(_tmp_3, _tmp_4);
}
#endif

ALWAYS_INLINE static inline __m128 _mm_broadcastminss_ps(__m128 _a) {
    __m128 _tmp_0 = _mm_permute_ps(_a, 0b10110001);
    __m128 _tmp_1 = _mm_min_ps(_a, _tmp_0);
    __m128 _tmp_2 = _mm_permute_ps(_tmp_1, 0b01001110);

    return _mm_min_ps(_tmp_1, _tmp_2);
}





/**
 * @brief  Recursive traverses a BVH containing either up to 4/8/16 child nodes
 *         or up to 4/8/16 triangles (depends on available instruction set).
 *         Calculates a possible intersection for a single ray.
 * @param  ray  The ray containing origin, a normalized direction and
 *             a precomputed normalized vec3_inverse direction.
 * @param  node BVH node containing either up to 4/8/16 AABBs or up to 4/8/16 triangles as node.
 * @param  hit  Address of the possible hit triangle for this specific ray.
 * @return Distance between the ray origin and the intersected triangle.
 *         Falls back to NO_INTERSECTION in case no intersection occurred.
 */
#if defined(__AVX512F__)
f32 traverse(const BVH_Ray *__restrict__ ray, const BVH_Node *__restrict__ node, Triangle **hit) {
    /* TODO */
}

#elif defined(__AVX2__)
f32 traverse(const BVH_Ray *__restrict__ ray, const BVH_Node *__restrict__ node, Triangle **hit) {
    if (__builtin_expect(!(node->metadata.bvh_cnt), 0)) {
        __m256 _hits = ray_triangle_intersection(ray, node);

        /// finding the smallest float and its index
        __m256 _tmp_0 = _mm256_broadcastminss_ps(_hits);
        __m256 _tmp_1 = _mm256_cmp_ps(_hits, _tmp_0, _CMP_EQ_OQ);

        // retrieving the index of the smallest float
        i32 idx = __builtin_ctz(_mm256_movemask_ps(_tmp_1));
        *hit = &ARRAY_ELEMENT(triangle_buffer, idx + node->metadata.tri_idx);

        // the hit distance is broadcast as min value
        return _mm256_cvtss_f32(_tmp_0);
    }
    else {
        __m256 _hits = ray_node_intersection(ray, node);
        __m256 _tmp_0 = _mm256_cmp_ps(_hits, _mm256_set1_ps(NO_INTERSECTION), _CMP_LT_OQ);

        // counting total amount of traversable elements
        i32 max = __builtin_popcount(_mm256_movemask_ps(_tmp_0));
        f32 scale = NO_INTERSECTION;

        for (i32 i = 0; i < max && scale == NO_INTERSECTION; ++i) {
            __m256 _tmp_1 = _mm256_broadcastminss_ps(_hits);
            __m256 _tmp_2 = _mm256_cmp_ps(_hits, _tmp_1, _CMP_EQ_OQ);

            // index where the nearest intersection resides
            i32 idx = __builtin_ctz(_mm256_movemask_ps(_tmp_2));

            // masking off the nearest intersection
            _hits = _mm256_blendv_ps(_hits, _mm256_set1_ps(NO_INTERSECTION), _tmp_2);

            // relative child node to be traversed
            BVH_Node *next = &ARRAY_ELEMENT(aabb_buffer, idx + node->metadata.bvh_idx);
            scale = traverse(ray, next, hit);
        }

        return scale;
    }
}

#else
f32 traverse(const BVH_Ray *__restrict__ ray, const BVH_Node *__restrict__ node, Triangle **hit) {
    /* TODO */
}

#endif

AABB mesh_aabb;

static i32 polygon_sort(const void *a, const void *b) {
    assert(a);
    assert(b);

    const Triangle _a = *(const Triangle *) a;
    const Triangle _b = *(const Triangle *) b;

    BITMAP(a_morton, 96);
    BITMAP(b_morton, 96);

    morton3d_f32(_a.centroid, &mesh_aabb, a_morton);
    morton3d_f32(_b.centroid, &mesh_aabb, b_morton);

    return morton3d_comparator(a_morton, b_morton);
}