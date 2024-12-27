//
// Created by Luis Ruisinger on 26.11.24.
//

#ifndef SOFTWARE_RATYTRACING_BVH_H
#define SOFTWARE_RATYTRACING_BVH_H

#include "../util/vec3.h"
#include "../util/ray.h"
#include "../model/polygon.h"

C_GUARD_BEGINN()

/**
 * Packed SOA representation of 8 triangles.
 */
typedef struct BVH_Triangles_t {

    // two edges of each triangle needed for the cross product
    // split up in x, y and z
    __m256 _edge_0[DIMENSIONS];
    __m256 _edge_1[DIMENSIONS];

    // first point of each triangle
    // split up in x, y and z
    __m256 _point_a[DIMENSIONS];

    // pointer to the attributes of each triangle
    Triangle *triangles;
} __attribute__((aligned(ALIGNMENT_256))) BVH_Triangles;





/**
 * Packed SOA representation of 8 aabbs.
 */
typedef struct BVH_AABBs_t {

    // all x,y and z parts of the vectors are packed together
    // allows processing of 8 nodes aabb nodes at the same time
    __m256 _min[DIMENSIONS];
    __m256 _max[DIMENSIONS];

    // pointer to 8 children
    struct BVH_Node_t *children;
} __attribute__((aligned(ALIGNMENT_256))) BVH_AABBs;





/**
 * Representation of a single BVH node.
 * Contains packed SOA representation of 8 aabb's it contains as children or
 * packed SOA representation of 8 triangles in which case it acts as a leaf of the BVH.
 */
typedef struct BVH_Node_t {
    union {

        // pointer to 8 aabbs
        BVH_AABBs aabbs;

        // pointer to 8 leaves
        BVH_Triangles triangles;
    };

    u32 mask;
} __attribute__((aligned(ALIGNMENT_256))) BVH_Node;

#define IS_LEAF(node) \
    ((usize) (node)->mask & 0x1)

#define SET_LEAF(node) \
    ((usize) (node)->mask | 0x1)

#define GET_CHILDREN(node) \
    ((node)->aabbs.children)

#define SET_CHILDREN(node, ptr) \
    do { (node)->aabbs.children = (ptr); } while (0)

#define GET_TRIANGLES(node) \
    ((node)->triangles.triangles)

#define SET_TRIANGLES(node, ptr)           \
    do {                                   \
        (node)->triangles.triangles = ptr; \
        (node)->mask |= 0x1;               \
    } while (0)





/**
 * The packed representation of a ray to calculate intersections on 8 aabb's / triangles
 * at the same time.
 * Used only inside the BVH. Needed to keep a consistent ray state rather than transforming
 * a ray into another representation at each intersection calculation.
 */
typedef struct BVH_Ray_T {

    // all x,y and z parts of the vectors are packed together
    // each vec3f is repeated 8 times
    // allows processing of 8 nodes aabb nodes at the same time
    __m256 _org[DIMENSIONS];
    __m256 _dir[DIMENSIONS];

    // precomputed inverse direction
    __m256 _inv_dir[DIMENSIONS];
} __attribute__((aligned(ALIGNMENT_256))) BVH_Ray;

#define BVH_RAY(org_vec, dir_vec, inv_dir_vec)                                                    \
    (BVH_Ray) {                                                                                   \
        ._org = {                                                                                 \
            _mm256_set1_ps(GET_VEC_X(org_vec)),                                                   \
            _mm256_set1_ps(GET_VEC_Y(org_vec)),                                                   \
            _mm256_set1_ps(GET_VEC_Z(org_vec))                                                    \
        },                                                                                        \
        ._dir = {                                                                                 \
            _mm256_set1_ps(GET_VEC_X(dir_vec)),                                                   \
            _mm256_set1_ps(GET_VEC_Y(dir_vec)),                                                   \
            _mm256_set1_ps(GET_VEC_Z(dir_vec))                                                    \
        },                                                                                        \
        ._inv_dir = {                                                                             \
            _mm256_set1_ps(GET_VEC_X(inv_dir_vec)),                                               \
            _mm256_set1_ps(GET_VEC_Y(inv_dir_vec)),                                               \
            _mm256_set1_ps(GET_VEC_Z(inv_dir_vec))                                                \
        }                                                                                         \
    }





void ray_child_nodes_intersection(const BVH_Ray *ray, const BVH_Node *node);

C_GUARD_END()

#endif //SOFTWARE_RATYTRACING_BVH_H
