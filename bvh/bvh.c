//
// Created by Luis Ruisinger on 26.11.24.
//

#include "bvh.h"
#include "../global.h"

#define DEPTH 16
#define NO_INTERSECTION INFINITY

/**
 * @brief  Calculates the distance between the ray origin and possible intersection
 *         with 8 packed aabbs in 3d space.
 *         In case no hit occurs for one particular aabb it's result
 *         would fall back to NO_INTERSECTION.
 * @param ray  The ray containing origin, a normalized direction and
 *             a precomputed normalized vec3_inverse direction.
 * @param  node Packed SOA of 8 aabbs.
 * @return __m256 packed result of 8 computed possible intersections.
 */
__m256 ray_node_intersection(const BVH_Ray *restrict ray, const BVH_Node *restrict node) {
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

static void ALWAYS_INLINE avx_multi_cross(
        __m256 _res[DIMENSIONS], const __m256 _a[DIMENSIONS], const __m256 _b[DIMENSIONS]) {
    _res[0] = _mm256_fmsub_ps(_a[1], _b[2], _mm256_mul_ps(_b[1], _a[2]));
    _res[1] = _mm256_fmsub_ps(_a[2], _b[0], _mm256_mul_ps(_b[2], _a[0]));
    _res[2] = _mm256_fmsub_ps(_a[0], _b[1], _mm256_mul_ps(_b[0], _a[1]));
}

static void ALWAYS_INLINE avx_multi_sub(
        __m256 _res[DIMENSIONS], const __m256 _a[DIMENSIONS], const __m256 _b[DIMENSIONS]) {
    _res[0] = _mm256_sub_ps(_a[0], _b[0]);
    _res[1] = _mm256_sub_ps(_a[1], _b[1]);
    _res[2] = _mm256_sub_ps(_a[2], _b[2]);
}

static __m256 ALWAYS_INLINE avx_multi_dot(
        const __m256 _a[DIMENSIONS], const __m256 _b[DIMENSIONS]) {
    __m256 _tmp_0 = _mm256_mul_ps(_a[0], _b[0]);
    __m256 _tmp_1 = _mm256_fmadd_ps(_a[1], _b[1], _tmp_0);

    return _mm256_fmadd_ps(_a[2], _b[2], _tmp_1);
}

/**
 * @brief  Calculates the distance between the ray origin and possible intersection
 *         with 8 packed triangles in 3d space.
 *         In case no hit occurs for one particular triangle it's result
 *         would fall back to NO_INTERSECTION.
 * @param ray  The ray containing origin, a normalized direction and
 *             a precomputed normalized vec3_inverse direction.
 * @param  node Packed SOA of 8 triangles.
 * @return __m256 packed result of 8 computed possible intersections.
 */
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

/**
 * @brief  Broadcasts the smallest single-precision float in an __m256 vector.
 * @param  _a Single-precision float vector.
 * @return __m256 vector containing the smallest single-precision float
 */
ALWAYS_INLINE static inline __m256 _mm256_broadcastminss_ps(__m256 _a) {
    __m256 _tmp_0 = _mm256_permute_ps(_a, 0b10110001);
    __m256 _tmp_1 = _mm256_min_ps(_a, _tmp_0);
    __m256 _tmp_2 = _mm256_permute_ps(_tmp_1, 0b01001110);
    __m256 _tmp_3 = _mm256_min_ps(_tmp_1, _tmp_2);

    /* TODO: the following line only compiles for AVX2 and not AVX or below */
    __m256 _tmp_4 = _mm256_castpd_ps(_mm256_permute4x64_pd(_mm256_castps_pd(_tmp_3), 0b01001110));

    return  _mm256_min_ps(_tmp_3, _tmp_4);
}

/**
 * @brief  Recursive traverses a BVH containing either up to 8 child nodes or up to 8 triangles.
 *         Calculates a possible intersection for a single ray.
 * @param  ray  The ray containing origin, a normalized direction and
 *             a precomputed normalized vec3_inverse direction.
 * @param  node BVH node containing either up to 8 aabbs or up to 8 triangles as node.
 * @param  hit  Address of the possible hit triangle for this specific ray.
 * @return Distance between the ray origin and the intersected triangle.
 *         Falls back to NO_INTERSECTION in case no intersection occured.
 */
f32 traverse(const BVH_Ray *__restrict__ ray, const BVH_Node *__restrict__ node, Triangle **hit) {
    if (__builtin_expect(GET_LEAF(node), 0)) {
        __m256 _hits = ray_triangle_intersection(ray, node);

        /// finding the smallest float and its index
        __m256 _tmp_0 = _mm256_broadcastminss_ps(_hits);
        __m256 _tmp_1 = _mm256_cmp_ps(_hits, _tmp_0, _CMP_EQ_OQ);

        // retrieving the index of the smallest float
        i32 idx = __builtin_ctz(_mm256_movemask_ps(_tmp_1));
        *hit = &ARRAY_ELEMENT(triangle_buffer, idx + node->packed_0);

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
            BVH_Node *next = &ARRAY_ELEMENT(aabb_buffer, idx + node->packed_0);
            scale = traverse(ray, next, hit);
        }

        return scale;
    }
}

// struct Tri { float3 vertex0, vertex1, vertex2; float3 centroid; };
// struct Ray { float3 O, D; float t = 1e30f; };

/*
 * struct BVHNode
{
float3 aabbMin, aabbMax;
uint leftFirst, triCount;
};
 */

// 0 - - - 4 - - -
// 0 - 2 - 4 - 6 - // must replace all nodes from step before
// 0 1 2 3 4 5 6 7 // must replace all nodes from step before

f32 eval_sah_splitplane(const BVH_Node *node, u32 axis, f32 pos) {
    const vec3f min = VEC3(1E30F);
    const vec3f max = VEC3(-1E30F);

    AABB left = AABB(min, max);
    AABB right = AABB(min, max);

    usize left_cnt = 0;
    usize right_cnt = 1;

    usize node_cnt = GET_NODE_CNT(node);
}
