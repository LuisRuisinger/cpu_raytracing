//
// Created by Luis Ruisinger on 24.11.24.
//

#ifndef SOFTWARE_RATYTRACING_COLOR_H
#define SOFTWARE_RATYTRACING_COLOR_H

#include "vec3.h"
#include "defines.h"

C_GUARD_BEGINN()

static ALWAYS_INLINE u32 vec3f_color_to_u32_color(vec3f color) {
    return
        (((u32) (GET_VEC3_X(color) * 255.0F) & 0xFF) << 24) |
        (((u32) (GET_VEC3_Y(color) * 255.0F) & 0xFF) << 16) |
        (((u32) (GET_VEC3_Z(color) * 255.0F) & 0xFF) <<  8) |
        0xFF;
}

static ALWAYS_INLINE vec3f u32_color_to_vec3f_color(u32 color) {
    f32 x = (f32) ((color >> 24) & 0xFF) / 255.0F;
    f32 y = (f32) ((color >> 16) & 0xFF) / 255.0F;
    f32 z = (f32) ((color >>  8) & 0xFF) / 255.0F;

    return VEC3(x, y, z);
}

C_GUARD_END()

#endif //SOFTWARE_RATYTRACING_COLOR_H
