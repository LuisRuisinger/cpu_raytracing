//
// Created by Luis Ruisinger on 24.11.24.
//

#ifndef SOFTWARE_RAYTRACING_VEC3_H
#define SOFTWARE_RAYTRACING_VEC3_H

#include <math.h>
#include <stdatomic.h>

#include "../defines.h"
#include "vec4.h"

C_GUARD_BEGINN()

typedef vec4f vec3f;

#define VEC3_PAD 0.0F

#define VEC3_1(_x) \
    VEC4_4(_x, _x, _x, VEC3_PAD)

#define VEC3_2(_x, _y) \
    VEC4_4(_x, _y, _y, VEC3_PAD)

#define VEC3_3(_x, _y, _z) \
    VEC4_4(_x, _y, _z, VEC3_PAD)

#define VEC3_GET_MACRO(_1, _2, _3, NAME, ...) NAME

#define VEC3(...) \
    VEC3_GET_MACRO(__VA_ARGS__, VEC3_3, VEC3_2, VEC3_1, INVALID)(__VA_ARGS__)

#define GET_VEC3_X(_v) \
    GET_VEC4_X(_v)

#define GET_VEC3_Y(_v) \
    GET_VEC4_Y(_v)

#define GET_VEC3_Z(_v) \
    GET_VEC4_Z(_v)

#define SET_VEC3_X(_v, _x) \
    SET_VEC4_X(_v, _x)

#define SET_VEC3_Y(_v, _y) \
    SET_VEC4_Y(_v, _y)

#define SET_VEC3_Z(_v, _z) \
    SET_VEC4_Z(_v, _z)

#define VEC3_SIGN(_v) \
    VEC4_SIGN(_v)

vec3f vec3_adds(vec3f a, f32 scalar);
vec3f vec3_add(vec3f a, vec3f b);
vec3f vec3_subs(vec3f a, f32 scalar);
vec3f vec3_sub(vec3f a, vec3f b);
vec3f vec3_muls(vec3f a, f32 scalar);
vec3f vec3_mul(vec3f a, vec3f b);
vec3f vec3_divs(vec3f a, f32 scalar);
f32 vec3_length(vec3f a);
vec3f vec3_normalize(vec3f a);
f32 vec3_dot(vec3f a, vec3f b);
vec3f vec3_cross(vec3f a, vec3f b);
vec3f vec3_inverse(vec3f a);
vec3f vec3_min(vec3f a, vec3f b);
vec3f vec3_max(vec3f a, vec3f b);
vec3f vec3_refract(vec3f i, vec3f n, f32 eta);
vec3f vec3_reflect(vec3f i, vec3f n);

C_GUARD_END()

#endif //SOFTWARE_RAYTRACING_VEC3_H
