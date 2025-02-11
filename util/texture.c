#include "texture.h"

typedef struct Texture_t {
    usize w;
    usize h;
    usize d;

    i32 min_filter;
    i32 mag_filter;
    i32 wrap_s;
    i32 wrap_t;
    i32 wrap_r;

    i32 format;
    i32 type;

    u8 *data;
} Texture;

#define BITWISE_U32(_type, _t) \
    ((union { u32 u; _type t; }) { .t = _t }.u)


/**
 * The idea is to store function pointers which get resolved through an i32 identifier.
 * E.g. SR_LINEAR gets resolved to linear interpolation for 1d textures and bilinear interpolation
 * for 2d textures. 
 * Therefore we can just store identifiers and their functions as value and resolve them during 
 * runtime to chain calls to resolve 32 bit values from various N-dimensional textures. 
 */
MAP_FUNC(i32, u32 (* fun)(u32, vec3f)) map;

#define TEXTURE_PEEK(_type) \
    static _type texture_peek_##_type(u32 tex_id, vec3f v) {                                      \
        Texture *tex = /* TODO */ NULL;                                                           \
                                                                                                  \
        usize entry_size = sizeof(_type);                                                         \
        usize column_len = tex->w * entry_size;                                                   \
        usize texture_len = column_len * tex->h;                                                  \
                                                                                                  \
        return *(_type *)                                                                         \
            ((u8 *) tex->data + texture_len * (u32) VEC3_GET(v, 2) +                              \
            column_len * (u32) VEC3_GET(v, 1) +  (u32) (u32) VEC3_GET(v, 0));                     \
    }

TEXTURE_PEEK(u8)
TEXTURE_PEEK(u16)
TEXTURE_PEEK(u32)
TEXTURE_PEEK(i8)
TEXTURE_PEEK(i16)
TEXTURE_PEEK(i32)
TEXTURE_PEEK(f32)

#define BILINEAR_INTERPOLATION(_type)                                                             \
    static u32 bilinear_interpolation_##_type(u32 tex_id, vec3f v) {                              \
        u32 x_fr = floor(VEC3_GET(v, 0));                                                         \
        u32 y_fr = floor(VEC3_GET(v, 1));                                                         \
        u32 x_cl = ceil(VEC3_GET(v, 0));                                                          \
        u32 y_cl = ceil(VEC3_GET(v, 1));                                                          \
                                                                                                  \
        vec4f rows[4] = {                                                                         \
            VEC4( (f32) (x_cl * y_cl), -(f32) y_cl, -(f32) x_cl,  1.0F),                          \
            VEC4(-(f32) (x_cl * y_fr),  (f32) y_fr,  (f32) x_cl, -1.0F),                          \
            VEC4(-(f32) (x_fr * y_cl),  (f32) y_cl,  (f32) x_fr, -1.0F),                          \
            VEC4( (f32) (x_fr * y_fr), -(f32) y_fr, -(f32) x_fr,  1.0F)                           \
        };                                                                                        \
                                                                                                  \
        Mat4x4 mat = mat4x4_from_rvec(rows);                                                      \
                                                                                                  \
        f32 q_ff = (f32) texture_peek_##_type(tex_id, VEC3(x_fr, y_fr, VEC3_GET(v, 2)));          \
        f32 q_fc = (f32) texture_peek_##_type(tex_id, VEC3(x_fr, y_cl, VEC3_GET(v, 2)));          \
        f32 q_cf = (f32) texture_peek_##_type(tex_id, VEC3(x_cl, y_fr, VEC3_GET(v, 2)));          \
        f32 q_cc = (f32) texture_peek_##_type(tex_id, VEC3(x_cl, y_cl, VEC3_GET(v, 2)));          \
                                                                                                  \
        vec4f c = VEC4(q_ff, q_fc, q_cf, q_cc);                                                   \
        vec4f w = mat4x4_mulv(&mat, c);                                                           \
                                                                                                  \
        f32 s = 1.0F / ((x_cl - x_fr) * (y_cl * y_fr));                                           \
        w = vec4_muls(w, s);                                                                      \
        c = vec4_mul(w, c);                                                                       \
                                                                                                  \
        return BITWISE_U32(f32, vec4_hsum(c));                                                    \
    }   

BILINEAR_INTERPOLATION(u8)
BILINEAR_INTERPOLATION(u16)
BILINEAR_INTERPOLATION(u32)
BILINEAR_INTERPOLATION(i8)
BILINEAR_INTERPOLATION(i16)
BILINEAR_INTERPOLATION(i32)
BILINEAR_INTERPOLATION(f32)

#define NEAREST(_type)                                                                            \
    static u32 nearest_##_type(u32 tex_id, vec3f v) {                                             \
        v = vec3_adds(v, 0.5F);                                                                   \
        return BITWISE_U32(_type, texture_peek_##_type(tex_id, v));                               \
    }

NEAREST(u8)
NEAREST(u16)
NEAREST(u32)
NEAREST(i8)
NEAREST(i16)
NEAREST(i32)
NEAREST(f32)

vec3f clamp(vec3f v) {

}

u32 sample_2d(u32 tex_id, vec2f v) {
    Texture *tex = /* TODO */ NULL;
    u32 (* fun)(u32, f32, f32) = NULL;
    
    MAP_GET(map, tex->wrap_s, fun);
    v.x = fun(tex_id, v.x, v.y);

    MAP_GET(map, tex->wrap_t, fun);
    v.y = fun(tex_id, v.x, v.y);
}



