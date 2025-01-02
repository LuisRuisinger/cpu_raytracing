//
// Created by Luis Ruisinger on 26.11.24.
//

#ifndef SOFTWARE_RATYTRACING_POLYGON_H
#define SOFTWARE_RATYTRACING_POLYGON_H

#include <float.h>

#include "../util/vec3.h"
#include "../bvh/aabb.h"
#include "../util/ray.h"
#include "../util/defines.h"

C_GUARD_BEGINN()

/**
 * Packed struct containing information about the shade of the triangle.
 * Packed size should exactly be 32 bytes.
 */
typedef struct Triangle_Shade_t {

    // 24 bytes + 8 bytes padding
    vec2f uv_a;
    vec2f uv_b;
    vec2f uv_c;

    // 8 bytes
    u32 static_color;
    u32 shade_type;
} __attribute__((packed, aligned(ALIGNMENT_256))) Triangle_Shade;

typedef enum Shade_t {
    FLAT       = 1 << 0,
    PER_VERTEX = 1 << 1,
    TEXTURED   = 1 << 2
} Shade;

#define IS_FLAT_SHADED(triangle) \
    ((triangle)->shade_type & FLAT)

#define IS_PER_VERTEX_SHADED(triangle) \
    ((triangle)->shade_type & PER_VERTEX)

#define IS_TEXTURED_SHADED(triangle) \
    ((triangle)->shade_type & TEXTURED)





/**
 * Representation of a triangle. The smallest geometric unit of this raytracing engine.
 */
typedef struct Triangle_t {

    // 32 bytes
    union {
        Triangle_Shade shade;
        u32 static_color;
    };

    // 32 bytes
    // aligned to 32
    vec3f point_b;
    vec3f point_c;

    // 32 bytes
    // aligned to 32
    vec3f point_a;
    vec3f normal;
} __attribute__((aligned(ALIGNMENT_256))) Triangle;

#define TRIANGLE_CENTROID(tri) \
    mul_scalar(add_vec(add_vec((tri).point_b, (tri).point.c), (tri).point.a)), 0.3333F)



void eval_surface_normal(Triangle *triangle);
void eval_centroid(Triangle *triangle);


C_GUARD_END()

#endif //SOFTWARE_RATYTRACING_POLYGON_H
