//
// Created by Luis Ruisinger on 26.11.24.
//

#ifndef SOFTWARE_RATYTRACING_BVH_H
#define SOFTWARE_RATYTRACING_BVH_H

#include "../util/datastructures/bitmap.h"
#include "../util/linalg/vec3.h"
#include "../util/ray.h"

#include "../model/polygon.h"

#if defined(__AVX512__)
    #define SOA_ALIGNMENT sizeof(__m512)
#elif defined(__AVX2__)
    #define SOA_ALIGNMENT sizeof(__m256)
#else
    #define SOA_ALIGNMENT sizeof(__m128)
#endif

C_GUARD_BEGINN()

/**
 * Packed SOA representation of 4/8/16 triangles.
 */
typedef struct BVH_Triangles_t {
#if defined(__AVX512F__)
    // two edges of each triangle needed for the vec3_cross product
    // split up in x, y and z
    __m512 _edge_0[DIMENSIONS];
    __m512 _edge_1[DIMENSIONS];

    // first point of each triangle
    // split up in x, y and z
    __m512 _point_a[DIMENSIONS];

#elif defined(__AVX2__)
    // two edges of each triangle needed for the vec3_cross product
    // split up in x, y and z
    __m256 _edge_0[DIMENSIONS];
    __m256 _edge_1[DIMENSIONS];

    // first point of each triangle
    // split up in x, y and z
    __m256 _point_a[DIMENSIONS];

#else
    // two edges of each triangle needed for the vec3_cross product
    // split up in x, y and z
    __m128 _edge_0[DIMENSIONS];
    __m128 _edge_1[DIMENSIONS];

    // first point of each triangle
    // split up in x, y and z
    __m128 _point_a[DIMENSIONS];
#endif
} __attribute__((aligned(SOA_ALIGNMENT))) BVH_Triangles;





/**
 * Packed SOA representation of 4/8/16 AABBs.
 */
typedef struct BVH_AABBs_t {
#if defined(__AVX512F__)
    // all x,y and z parts of the vectors are packed together
    // allows processing of 4 nodes aabb nodes at the same time
    __m512 _min[DIMENSIONS];
    __m512 _max[DIMENSIONS];

#elif defined(__AVX2__)
    // all x,y and z parts of the vectors are packed together
    // allows processing of 8 nodes aabb nodes at the same time
    __m256 _min[DIMENSIONS];
    __m256 _max[DIMENSIONS];

#else
    // all x,y and z parts of the vectors are packed together
    // allows processing of 16 nodes aabb nodes at the same time
    __m128 _min[DIMENSIONS];
    __m128 _max[DIMENSIONS];

#endif
} __attribute__((aligned(SOA_ALIGNMENT))) BVH_AABBs;





/**
 * Representation of a single BVH node.
 * Contains packed SOA representation of 8 aabb's it contains as node or
 * packed SOA representation of 8 triangles in which case it acts as a leaf of the BVH.
 */
typedef struct BVH_Node_t {

    // relative offset into a linear buffer containing bvh nodes
    u32 bvh_idx;

    // relative offset into a linear buffer containing triangles
    u32 tri_idx;

    // containing amount of triangles this aabb encompasses
    u32 packed;

    union {
        BVH_AABBs     aabbs;
        BVH_Triangles triangles;
    };
} __attribute__((aligned(SOA_ALIGNMENT))) BVH_Node;

#define LEAF_MASK (1UL << 3)
#define NODE_CNT_MASK 7UL

#define IS_LEAF(_node) \
    (!!(_node)->packed)

#define GET_NODE_CNT(_node) \
    ((_node)->packed)

#define SET_NODE_CNT(_node, _cnt) \
    ((_node)->packed = (_cnt))





/**
 * The packed representation of a ray to calculate intersections on 8 aabb's / triangles
 * at the same time.
 * Used only inside the BVH. Needed to keep a consistent ray state rather than transforming
 * a ray into another representation at each intersection calculation.
 */
typedef struct BVH_Ray_T {
#if defined(__AVX512F__)
    // all x,y and z parts of the vectors are packed together
    // each vec3f is repeated 16 times
    // allows processing of 16 nodes aabb nodes at the same time
    __m512 _org[DIMENSIONS];
    __m512 _dir[DIMENSIONS];

    // precomputed vec3_inverse direction
    __m512 _inv_dir[DIMENSIONS];

#elif defined(__AVX2__)
    // all x,y and z parts of the vectors are packed together
    // each vec3f is repeated 8 times
    // allows processing of 8 nodes aabb nodes at the same time
    __m256 _org[DIMENSIONS];
    __m256 _dir[DIMENSIONS];

    // precomputed vec3_inverse direction
    __m256 _inv_dir[DIMENSIONS];

#else
    // all x,y and z parts of the vectors are packed together
    // each vec3f is repeated 4 times
    // allows processing of 4 nodes aabb nodes at the same time
    __m128 _org[DIMENSIONS];
    __m128 _dir[DIMENSIONS];

    // precomputed vec3_inverse direction
    __m128 _inv_dir[DIMENSIONS];

#endif
} __attribute__((aligned(SOA_ALIGNMENT))) BVH_Ray;




BVH_Node *bvh_build(const char **, usize);
BVH_Node *bvh_build_fast(const char **, usize);
f32 bvh_traverse(const BVH_Ray *__restrict__, const BVH_Node *__restrict__, Triangle **);

C_GUARD_END()

#endif //SOFTWARE_RATYTRACING_BVH_H
