//
// Created by Luis Ruisinger on 30.11.24.
//

#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "obj_parser.h"
#include "../util/datastructures/dict.h"
#include "../util/vec3.h"
#include "../global.h"
#include "../util/fmt.h"

#define FAST_OBJ_IMPLEMENTATION
#include "../util/fast_obj/fast_obj.h"

i32 parse(const char *file, TriangleArr *arr) {\
    fastObjMesh *mesh = fast_obj_read("../model/test/cube.obj");
    if (!mesh) {
        return -1;
    }

    ARRAY_INIT(*arr, mesh->index_count / 3);

    for (usize i = 0; i < mesh->index_count; i += 3) {
        Triangle triangle = { 0 };

        vec3f pos[3] = { VEC3(0.0F) };
        vec2f tex[3] = { VEC2(0.0F) };
        u32   col[3] = { 0UL };

        for (usize j = 0; j < 3; ++j) {
            fastObjIndex *mesh_idx = &mesh->indices[i + j];

            // position
            {
                f32 x = mesh->positions[3 * mesh_idx->p + 0];
                f32 y = mesh->positions[3 * mesh_idx->p + 1];
                f32 z = mesh->positions[3 * mesh_idx->p + 2];
                triangle.point[j] = VEC3(x, y, z);
            }

            // texture
            if (mesh->texcoord_count) {
                f32 x = mesh->positions[2 * mesh_idx->t + 0];
                f32 y = mesh->positions[2 * mesh_idx->t + 1];
                triangle.tex.uv[j] = VEC2(x, y);

                SET_TEX_COORDS(triangle);
            }
            else {
                /* TODO */
                // how can we even store per vertex color (rather than per face) in .obj files
            }

        }

        eval_surface_normal(&triangle);
        eval_centroid(&triangle);

        ARRAY_PUSH_BACK(*arr, triangle);
    }

    fast_obj_destroy(mesh);
    return 0;
}



