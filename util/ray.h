//
// Created by Luis Ruisinger on 26.11.24.
//

#ifndef SOFTWARE_RAYTRACING_RAY_H
#define SOFTWARE_RAYTRACING_RAY_H

#include "defines.h"
#include "vec3.h"

C_GUARD_BEGINN()

typedef struct Ray_t {
    vec3f org;
    vec3f dir;
    vec3f inv_dir;
    vec3f __pad;
} __attribute__((aligned(ALIGNMENT_256))) Ray;

#define RAY(org_vec, dir_vec)        \
    (Ray) {                          \
        .org = (org_vec),            \
        .dir = (dir_vec),            \
        .inv_dir = inverse(dir_vec), \
        .__pad = VEC3(VEC3_PAD)      \
    }

C_GUARD_END()

#endif //SOFTWARE_RAYTRACING_RAY_H
