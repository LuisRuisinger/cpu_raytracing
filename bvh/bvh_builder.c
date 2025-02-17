//
// Created by Luis Ruisinger on 26.01.25.
//

#include <assert.h>
#include <pthread.h>

#include "bvh_builder.h"

#include "../util/fmt.h"
#include "../util/morton.h"
#include "../util/datastructures/pair.h"
#include "../util/threading/threadpool.h"

#define SEARCH_RADIUS 14

// static storage for a cube encompassing all polygons of a set of meshes
static AABB mesh_aabb;

// static storage for a tp being used for construction of the N-ary BVH
static Threadpool *tp;
static pthread_barrier_t barrier;

static i32 polygon_sort(const void *a, const void *b) {
    assert(a);
    assert(b);

    const Triangle _a = *(const Triangle *) a;
    const Triangle _b = *(const Triangle *) b;

    u64 morton_a = morton_vec(_a.centroid, &mesh_aabb);
    u64 morton_b = morton_vec(_b.centroid, &mesh_aabb);

    if (morton_a > morton_b) return 1;
    if (morton_a < morton_b) return -1;
    return 0;
}

/**
 * An intermediate representation of a BVH Node candidate.
 * Contains just a primitive or 2 children and acts basically as fallback to a 2-ary BVH
 * during construction of the N-ary BVH. 
 * 
 * Declared as candidate because other nodes can collpase into this node or this node is
 * being collapsed with M other nodes into a N-ary BVH node. 
 */
typedef struct BVH_Build_Node_t {
    AABB aabb;
    usize prim_cnt;
    usize fcp;
} BVH_Build_Node;

// forward declaration of a return type 
typedef PAIR(usize, usize) usize_p;

static usize_p range(usize i, usize beg, usize end) {
    return (usize_p) {
        .p0 = i > beg + SEARCH_RADIUS ? i - SEARCH_RADIUS : beg,
        .p1 = MIN(i + SEARCH_RADIUS + 1, end)
    };
}

static usize_p cluster(
    ARRAY(BVH_Build_Node) *in, ARRAY(BVH_Build_Node) *out,
    ARRAY(usize) *neighbors, usize merged_idx,
    usize beg, usize end, usize prev) {

    const Thread *thread_mdata = threadpool_mdata();
    usize thread_id = thread_mdata->id;
    usize thread_cnt = thread_mdata->thread_cnt;

    usize batch_size = (end - beg) / thread_cnt;
    usize batch_beg = beg + thread_id * batch_size;
    usize batch_end = batch_beg + batch_size;

    /* TODO */
    f32 *distances = malloc((SEARCH_RADIUS + 1) * SEARCH_RADIUS * sizeof(*distances));
    f32 **distance_mat = malloc((SEARCH_RADIUS + 1) * sizeof(f32 *));

    for (usize i = 0; i < SEARCH_RADIUS; ++i) {
        distance_mat[i] = &distances[i * SEARCH_RADIUS];
    }

    for (usize i = range(batch_beg, beg, end).p0; i < batch_beg; ++i) {
        usize range_end = range(i, beg, end).p1;

        for (usize j = i + 1; j < range_end; ++j) {
            aabb_grow_aabb(&ARRAY_ELEMENT(*in, i).aabb, &ARRAY_ELEMENT(*in, j).aabb);

            f32 half_area = aabb_area(&ARRAY_ELEMENT(*in, i).aabb) / 2.0F;
            distance_mat[batch_beg - i][j - i - 1] = half_area;
        }
    }

    pthread_barrier_wait(&barrier);

    for (usize i = batch_beg; i < batch_end; ++i) {
        usize_p p = range(i, beg, end);
        f32 best_distance = FLT_MAX; 
        usize best_neighbor = (usize) -1;

        for (usize j = p.p0; j < i; ++j) {
            f32 distance = distance_mat[i - j][i - j - 1];

            if (distance < best_distance) {
                best_distance = distance;
                best_neighbor = j;
            }
        }

        for (usize j = i + 1; j < p.p1; ++j) {
            aabb_grow_aabb(&ARRAY_ELEMENT(*in, i).aabb, &ARRAY_ELEMENT(*in, j).aabb);

            f32 distance = aabb_area(&ARRAY_ELEMENT(*in, i).aabb) / 2.0F;
            distance_mat[0][j - i - 1] = distance;

            if (distance < best_distance) {
                best_distance = distance;
                best_neighbor = j;
            }
        }

        assert(best_neighbor != (usize) -1);
        ARRAY_ELEMENT(*neighbors, i) = best_neighbor;

        f32 *lst = distance_mat[SEARCH_RADIUS];

        f32 **iter_fst = &distance_mat[0];
        f32 **iter_lst = &distance_mat[SEARCH_RADIUS];
        f32 **iter_nxt = &distance_mat[SEARCH_RADIUS + 1];
        while (iter_fst != iter_lst) {
            *(--iter_nxt) = *(--iter_lst);
        }

        distance_mat[0] = lst;
    }

    pthread_barrier_wait(&barrier);

    /* TODO */
    // seems like apple has no phtread barrier :DDDDDDDDD

    for (usize i = beg; i < end; ++i) {
        usize j = ARRAY_ELEMENT(*neighbors, i);
        bool mergeable = ARRAY_ELEMENT(*neighbors, j) == i;

        ARRAY_ELEMENT(*neighbors, merged_idx + i) = (i < j && mergeable) ? 1 : 0;
    }

    /* TODO */
    // prefix sum

    free(distance_mat);
    free(distances);

    /* TODO */
    return (usize_p) { .p0 = beg, .p1 = end };
}

static BVH_Node *compress(ARRAY(BVH_Build_Node) *nodes) {
    /* TODO */
}

/**
 * @brief  Calculates the aabb with encompasses all primitives of the given dynamic array.
 *         If you would calculate the AABB of any primitive afterwards it would be a true
 *         subregion of the here calculated "global" AABB. 
 * @param  tris Dynamic array containing all primitives for which we calculate the AABB. 
 * @return The resulting AABB. 
 */
static AABB calc_mesh_aabb(ARRAY(Triangle) *tris) {
    AABB aabb = AABB(VEC3(INFINITY), VEC3(-INFINITY));

    Triangle *tri = NULL;
    ARRAY_FOREACH(*tris, tri) {
        aabb_grow_vec(&aabb, tri->point[0]);
        aabb_grow_vec(&aabb, tri->point[1]);
        aabb_grow_vec(&aabb, tri->point[2]);
    }

    return aabb;
}

BVH_Node *bvh_build(void *args) {
    ARRAY(Triangle) *tris = args;

    // global AABB in which we normalize all floats to calculate morton codes
    // [v, w] -> [0, 1]^3 where v, w in R^3
    mesh_aabb = calc_mesh_aabb(tris);

    /* TODO */
    // enqueue into thread pool
    ARRAY_SORT(*tris, polygon_sort);

    usize node_cnt = 2 * tris->size - 1;
    ARRAY(BVH_Build_Node) nodes, nodes_cpy;
    ARRAY_INIT(nodes, node_cnt);
    ARRAY_INIT(nodes_cpy, node_cnt);

    ARRAY(usize) aux_data;
    ARRAY_INIT(aux_data, node_cnt * 3);

    usize beg = node_cnt - tris->size;
    usize end = node_cnt;
    usize prv = end;

    for (usize i = 0; i < tris->size; ++i) {
        BVH_Build_Node *b_node = &ARRAY_ELEMENT(nodes, beg + i);
        Triangle *tri = &ARRAY_ELEMENT(*tris, i);

        b_node->aabb = AABB(VEC3(INFINITY), VEC3(-INFINITY));
        b_node->prim_cnt = 1;    
        b_node->fcp = i; 

        aabb_grow_vec(&b_node->aabb, tri->point[0]);
        aabb_grow_vec(&b_node->aabb, tri->point[1]);
        aabb_grow_vec(&b_node->aabb, tri->point[2]); 
    }

    usize thread_cnt = MAX(hardware_concurrency(), 2);
    tp = threadpool_create(thread_cnt);
    pthread_barrier_init(&barrier, NULL, thread_cnt);

    usize *_end, *_beg;
    while (end - beg > 1) {
        usize_p p = cluster(&nodes, &nodes_cpy, &aux_data, node_cnt, beg, end, prv);
        ARRAY_SWAP(nodes_cpy, nodes);

        prv = end;
        beg = p.p0;
        end = p.p1;
    }

    /* TODO */
    // compress 
    BVH_Node *root = compress(&nodes);
    threadpool_destroy(tp);
    //pthread_barrier_destroy(&barrier);

    LOG("finished build");
    return root;
}
