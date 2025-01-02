//
// Created by Luis Ruisinger on 26.11.24.
//

#ifndef SOFTWARE_RATYTRACING_AABB_H
#define SOFTWARE_RATYTRACING_AABB_H

#include "../util/vec3.h"
#include "../util/ray.h"

C_GUARD_BEGINN()

typedef struct AABB_t {
    vec3f min;
    vec3f max;
} __attribute__((aligned(ALIGNMENT_256))) AABB;

#define AABB_1(v_max) \
    (AABB) { .min = VEC3(0.0F), .max = v_max }

#define AABB_2(v_min, v_max) \
    (AABB) { .min = v_min, .max = v_max }

#define AABB_GET_MACRO(_1, _2, NAME, ...) NAME

#define AABB(...) \
    AABB_GET_MACRO(__VA_ARGS__, AABB_2, AABB_1)(__VA_ARGS__)

void scale_vec(AABB *aabb, vec3f vec);
void scale_scalar(AABB *aabb, f32 scalar);
void scale_center_vec(AABB *aabb, vec3f vec);
void scale_center_scalar(AABB *aabb, f32 scalar);
void translate_vec(AABB *aabb, vec3f vec);
void translate_scalar(AABB *aabb, f32 scalar);
void aabb_grow_vec(AABB *aabb, vec3f v);
void aabb_grow_aabb(AABB *aabb, const AABB *other);

bool aabb_aabb_intersection(const AABB *a, const AABB *b);
bool ray_aabb_intersection(const Ray *ray, const AABB *aabb);

#define AABB_INTERSECTION(a, b) \
    _Generic((b), AABB *: aabb_aabb_intersection, Ray *: ray_aabb_intersection)(a, b)

vec3f aabb_aabb_intersection_distance(const AABB *a, const AABB *b);
vec3f ray_aabb_intersection_distance(const Ray *ray, const AABB *aabb);

#define AABB_INTERSECTION_DISTANCE(a, b) \
    _Generic((b),                        \
        AABB *: aabb_aabb_intersection_distance, \
        Ray *: ray_aabb_intersection_distance)(a, b)

C_GUARD_END()

#endif //SOFTWARE_RATYTRACING_AABB_H
