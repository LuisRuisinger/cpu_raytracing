//
// Created by Luis Ruisinger on 06.01.25.
//

#ifndef CPU_RAYTRACING_MATRIX_H
#define CPU_RAYTRACING_MATRIX_H

#include "../defines.h"
#include "vec3.h"

C_GUARD_BEGINN()

typedef struct Matrix_4x4_t {
    __m128 rows[4];
} __attribute__((aligned(16))) Matrix_4x4;

vec4f matrix_4x4_mulv(const Matrix_4x4 *mat, vec4f v);
Matrix_4x4 matrix_4x4_muls(const Matrix_4x4 *mat, f32 s);
Matrix_4x4 matrix_4x4_transpose(const Matrix_4x4 *mat);
Matrix_4x4 matrix_4x4_mulm(const Matrix_4x4 *a, const Matrix_4x4 *b);

C_GUARD_END()

#endif //CPU_RAYTRACING_MATRIX_H
