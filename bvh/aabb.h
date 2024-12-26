//
// Created by Luis Ruisinger on 26.11.24.
//

#ifndef SOFTWARE_RATYTRACING_AABB_H
#define SOFTWARE_RATYTRACING_AABB_H

#include "../util/vec3.h"
#include "../util/ray.h"

typedef struct AABB_t {
    vec3f min;
    vec3f max;
} __attribute__((aligned(ALIGNMENT))) AABB;

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

bool single_aabb_aabb_intersection(const AABB *a, const AABB *b);

bool single_ray_aabb_intersection(const Ray *ray, const AABB *aabb);

__m256 dual_ray_aabb_intersection(const Ray *ray, const AABB aabb[2]);

#endif //SOFTWARE_RATYTRACING_AABB_H
