//
// Created by Luis Ruisinger on 01.01.25.
//

#ifndef CPU_RAYTRACING_GLOBAL_H
#define CPU_RAYTRACING_GLOBAL_H

#include "util/datastructures/dynamic_array.h"
#include "model/polygon.h"
#include "bvh/bvh.h"

extern ARRAY(Triangle) triangle_buffer;
extern ARRAY(BVH_Node) aabb_buffer;

#endif //CPU_RAYTRACING_GLOBAL_H
