//
// Created by Luis Ruisinger on 06.01.25.
//

#ifndef CPU_RAYTRACING_VEC2_H
#define CPU_RAYTRACING_VEC2_H

#include "../defines.h"

typedef struct vec2f_t {
    f32 x;
    f32 y;
} __attribute__((aligned(8))) vec2f;

#define VEC2_1(_x) \
    (vec2f) { .x = _x, .y = _x }

#define VEC2_2(_x, _y) \
    (vec2f) { .x = _x, .y = _y }

#define VEC2_GET_MACRO(_1, _2, NAME, ...) NAME

#define VEC2(...) \
    VEC2_GET_MACRO(__VA_ARGS__, VEC2_2, VEC2_1, INVALID)(__VA_ARGS__)

#define GET_VEC2_X(_v) \
    (_v).x

#define GET_VEC2_Y(_v) \
    (_v).y

#define SET_VEC2_X(_v, _x) \
    ((_v).x = _x)

#define SET_VEC2_Y(_v, _y) \
    ((_v).y = _y)

#define VEC2_SIGN(_v) \
    (((GET_VEC2_X(_v) < 0) || (GET_VEC2_Y(_v) < 0)) ? -1 : +1)

vec2f vec2_adds(vec2f a, f32 scalar);
vec2f vec2_add(vec2f a, vec2f b);
vec2f vec2_subs(vec2f a, f32 scalar);
vec2f vec2_sub(vec2f a, vec2f b);
vec2f vec2_muls(vec2f a, f32 scalar);
vec2f vec2_mul(vec2f a, vec2f b);
vec2f vec2_divs(vec2f a, f32 scalar);
f32 vec2_length(vec2f a);
vec2f vec2_normalize(vec2f a);
f32 vec2_dot(vec2f a, vec2f b);
vec2f vec2_cross(vec2f a, vec2f b);
vec2f vec2_inverse(vec2f a);
vec2f vec2_min(vec2f a, vec2f b);
vec2f vec2_max(vec2f a, vec2f b);
vec2f vec2_refract(vec2f i, vec2f n, f32 eta);
vec2f vec2_reflect(vec2f i, vec2f n);

#endif //CPU_RAYTRACING_VEC2_H
