//
// Created by Luis Ruisinger on 26.11.24.
//

#ifndef SOFTWARE_RATYTRACING_POLYGON_H
#define SOFTWARE_RATYTRACING_POLYGON_H

#include <float.h>

#include "../util/linalg/vec3.h"
#include "../util/linalg/vec2.h"
#include "../bvh/aabb.h"
#include "../util/ray.h"
#include "../util/defines.h"
#include "../util/datastructures/dynamic_array.h"

C_GUARD_BEGINN()

/**
 * Struct containing information about the texture shading of the triangle.
 * Aligned and sized to a multiple of ALIGNMENT_256.
 */
typedef struct Triangle_Shade_t {

    // 24 bytes
    vec2f uv[DIMENSIONS];

    // 4 bytes
    u32 tex_id;
} __attribute__((aligned(ALIGNMENT_256))) Triangle_Shade;





/**
 * Struct containing information about the color of each triangle vertex.
 */
typedef struct Triangle_Color_t {
    u32 color[DIMENSIONS];
} Triangle_Color;





/**
 * Representation of a triangle. The smallest geometric unit of this raytracing engine.
 * Aligned and sized to a multiple of ALIGNMENT_256. Should be sized to 2 cache lines.
 */
typedef struct Triangle_t {

    // 32 bytes
    // aligned to 32
    union {
        Triangle_Shade tex;
        Triangle_Color col;
    };

    /* TODO */
    // how long is this ????
    // aligned to 32
    vec3f point[3];
    vec3f normal;
    vec3f centroid;
    f32 self_dot[3];
    f32 dot[3];
    f32 denom;

    u32 mask;
} __attribute__((aligned(ALIGNMENT_256))) Triangle;

#define HAS_TEX_COORDS(_tri) \
    (!!((_tri).mask & 1))

#define SET_TEX_COORDS(_tri) \
    ((_tri).mask |= 1)

/**
 * C cannot pass unnamed structs as function parameters because of ambiguity.
 * Therefore we define a named array struct especially for triangles.
 */
typedef ARRAY(Triangle) TriangleArr;


void eval_surface_normal(Triangle *triangle);
void eval_centroid(Triangle *triangle);
vec2f triangle_calculate_intersection_2d(Triangle *tri, vec3f hit);

C_GUARD_END()

#endif //SOFTWARE_RATYTRACING_POLYGON_H
