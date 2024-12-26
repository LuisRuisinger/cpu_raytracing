//
// Created by Luis Ruisinger on 26.11.24.
//

#ifndef SOFTWARE_RATYTRACING_RAY_H
#define SOFTWARE_RATYTRACING_RAY_H

typedef struct Ray_t {
    vec3f org;
    vec3f dir;
    vec3f inv_dir;
    vec3f __pad;
} __attribute__((aligned(ALIGNMENT))) Ray;

#define RAY(org_vec, dir_vec)        \
    (Ray) {                          \
        .org = (org_vec),            \
        .dir = (dir_vec),            \
        .inv_dir = inverse(dir_vec), \
        .__pad = VEC3(VEC3_PAD)      \
    }

#endif //SOFTWARE_RATYTRACING_RAY_H
