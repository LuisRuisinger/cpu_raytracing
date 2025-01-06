//
// Created by Luis Ruisinger on 06.01.25.
//

#ifndef CPU_RAYTRACING_MATRIX_H
#define CPU_RAYTRACING_MATRIX_H

#include "../defines.h"
#include "vec3.h"

C_GUARD_BEGINN()

typedef struct Mat4x4_t {
    alignas(16) f32 val[16];
} Mat4x4;

vec4f matrix_4x4_mulv(const Mat4x4 *mat, vec4f v);
void matrix_4x4_muls(const Mat4x4 *mat, Mat4x4 *dst, f32 s);
void matrix_4x4_transpose(const Mat4x4 *mat, Mat4x4 *dst);
void matrix_4x4_mulm(const Mat4x4 *a, const Mat4x4 *b, Mat4x4 *c);

C_GUARD_END()

#endif //CPU_RAYTRACING_MATRIX_H
