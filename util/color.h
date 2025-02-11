#ifndef SOFTWARE_RATYTRACING_COLOR_H
#define SOFTWARE_RATYTRACING_COLOR_H

#include "defines.h"
#include "linalg/vec3.h"
#include "linalg/matrix.h"

C_GUARD_BEGINN()

ALWAYS_INLINE static inline u32 vec3f_color_to_u32_color(vec3f color) {
    return
        (((u32) (VEC3_GET(color, 0) * 255.0F) & 0xFF) << 24) |
        (((u32) (VEC3_GET(color, 1) * 255.0F) & 0xFF) << 16) |
        (((u32) (VEC3_GET(color, 2) * 255.0F) & 0xFF) <<  8) |
        0xFF;
}

ALWAYS_INLINE static inline vec3f u32_color_to_vec3f_color(u32 color) {
    f32 x = (f32) ((color >> 24) & 0xFF) / 255.0F;
    f32 y = (f32) ((color >> 16) & 0xFF) / 255.0F;
    f32 z = (f32) ((color >>  8) & 0xFF) / 255.0F;

    return VEC3(x, y, z);
}

ALWAYS_INLINE static inline u32 bilinear_interpolation(u32 tex_id, vec2f uv) {
    u32 x_fr = floor(uv.x);
    u32 y_fr = floor(uv.y);
    u32 x_cl = ceil(uv.x);
    u32 y_cl = ceil(uv.y);

    vec4f rows[4] = {
        VEC4( (f32) (x_cl * y_cl), -(f32) y_cl, -(f32) x_cl,  1.0F),
        VEC4(-(f32) (x_cl * y_fr),  (f32) y_fr,  (f32) x_cl, -1.0F),
        VEC4(-(f32) (x_fr * y_cl),  (f32) y_cl,  (f32) x_fr, -1.0F),
        VEC4( (f32) (x_fr * y_fr), -(f32) y_fr, -(f32) x_fr,  1.0F)
    };

    Mat4x4 mat = mat4x4_from_rvec(rows);

    f32 q_ff = (f32) texture_peek(tex_id, x_fr, y_fr);
    f32 q_fc = (f32) texture_peek(tex_id, x_fr, y_cl);
    f32 q_cf = (f32) texture_peek(tex_id, x_cl, y_fr);
    f32 q_cc = (f32) texture_peek(tex_id, x_cl, y_cl);
    
    vec4f c = VEC4(q_ff, q_fc, q_cf, q_cc);
    vec4f w = mat4x4_mulv(&mat, c);

    f32 s = 1.0F / ((x_cl - x_fr) * (y_cl * y_fr));
    w = vec4_muls(w, s);
    c = vec4_mul(w, c);

    return (u32) vec4_hsum(c);
}

C_GUARD_END()

#endif //SOFTWARE_RATYTRACING_COLOR_H
