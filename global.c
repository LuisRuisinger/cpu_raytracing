//
// Created by Luis Ruisinger on 01.01.25.
//

#include "util/datastructures/dynamic_array.h"
#include "model/polygon.h"
#include "bvh/bvh.h"

ARRAY(Triangle) triangle_buffer;
ARRAY(BVH_Node) aabb_buffer;

void init_globals(void) {
    ARRAY_INIT(triangle_buffer, 32768);
    ARRAY_INIT(aabb_buffer, 32768);
}
