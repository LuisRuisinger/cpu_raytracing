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

    i32 tex_type;
    i32 format;
    i32 data_type;

    u8 *data;
} Texture;

// needed because the presence of a flaot is not guaranteed using an address cast
#define BITWISE_U32(_type, _t) \
    ((union { u32 u; _type t; }) { .t = _t }.u)

#define BITWISE_TYPE(_type, _u) \
    ((union { u32 u; _type t; }) { .u = _u }.t)

/**
 * The idea is to store function pointers which get resolved through an i32 identifier.
 * E.g. SR_LINEAR gets resolved to linear interpolation for 1d textures and bilinear interpolation
 * for 2d textures. 
 * Therefore we can just store identifiers and their functions as value and resolve them during 
 * runtime to chain calls to resolve 32 bit values from various N-dimensional textures. 
 */
MAP_FUNC(i32, u32 (* fun)(u32, vec3f)) map;

#define TEXTURE_PEEK(_type) \
    static u32 texture_peek_##_type(u32 tex_id, vec3f v) {                                        \
        Texture *tex = /* TODO */ NULL;                                                           \
                                                                                                  \
        usize entry_size = sizeof(_type);                                                         \
        usize column_len = tex->w * entry_size;                                                   \
        usize texture_len = column_len * tex->h;                                                  \
                                                                                                  \
        _type t = *(_type *)                                                                      \
            ((u8 *) tex->data + texture_len * (u32) VEC3_GET(v, 2) +                              \
            column_len * (u32) VEC3_GET(v, 1) +  (u32) (u32) VEC3_GET(v, 0));                     \
                                                                                                  \
        return BITWISE_U32(_type, t);                                                             \
    }

TEXTURE_PEEK(u8)
TEXTURE_PEEK(u16)
TEXTURE_PEEK(u32)
TEXTURE_PEEK(i8)
TEXTURE_PEEK(i16)
TEXTURE_PEEK(i32)
TEXTURE_PEEK(f32)

#define LINEAR(_type) \
    static u32 linear_##_type(u32 tex_id, vec3f v) { return 0UL; }

LINEAR(u8)
LINEAR(u16)
LINEAR(u32)
LINEAR(i8)
LINEAR(i16)
LINEAR(i32)
LINEAR(f32)

#define BILINEAR(_type)                                                                           \
    static u32 bilinear_##_type(u32 tex_id, vec3f v) {                                            \
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
        f32 q_ff = (f32) BITWISE_TYPE(                                                            \
            _type, texture_peek_##_type(tex_id, VEC3(x_fr, y_fr, VEC3_GET(v, 2))));               \
        f32 q_fc = (f32) BITWISE_TYPE(                                                            \
            _type, texture_peek_##_type(tex_id, VEC3(x_fr, y_cl, VEC3_GET(v, 2))));               \
        f32 q_cf = (f32) BITWISE_TYPE(                                                            \
            _type, texture_peek_##_type(tex_id, VEC3(x_cl, y_fr, VEC3_GET(v, 2))));               \
        f32 q_cc = (f32) BITWISE_TYPE(                                                            \
            _type, texture_peek_##_type(tex_id, VEC3(x_cl, y_cl, VEC3_GET(v, 2))));               \
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

BILINEAR(u8)
BILINEAR(u16)
BILINEAR(u32)
BILINEAR(i8)
BILINEAR(i16)
BILINEAR(i32)
BILINEAR(f32)

#define NEAREST(_type)                                                                            \
    static u32 nearest_##_type(u32 tex_id, vec3f v) {                                             \
        v = vec3_adds(v, 0.5F);                                                                   \
        return texture_peek_##_type(tex_id, v);                                                   \
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

u32 texture_sample(u32 tex_id, vec3f v) {
    Texture *tex = /* TODO */ NULL;

    u32 mask = tex->tex_type | tex->min_filter | tex->format | tex->data_type;
    u32 (* fun)(u32, vec3f) = NULL;
    
    MAP_GET(map, mask, fun);
    return fun(tex_id, v);
}

// masks off entire texture 
// because every texture and texture parameter is a unique bit 
// logical AND-ing this mask will result in true as a subset will be always set
// through this we can match against every type of texture as kind of wildcard
#define TEXTURE_MASK \
    (SR_TEXTURE_1D | SR_TEXTURE_1D_ARRAY | SR_TEXTURE_2D | SR_TEXTURE_2D_ARRAY)

// all texture types in 2D space (e.g. 1D texture array)
#define TEXTURE_MASK_2D \
    (SR_TEXTURE_1D_ARRAY | SR_TEXTURE_2D)

void texture_setup(void) {
    MAP_TYPEOF(map) arr[] = {

        // function pointers to nearest
        { .key = TEXTURE_MASK | SR_NEAREST | SR_RGB  | SR_BYTE,           .fun = nearest_i32 },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_BGR  | SR_BYTE,           .fun = nearest_i32 },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_RGBA | SR_BYTE,           .fun = nearest_i32 },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_BGRA | SR_BYTE,           .fun = nearest_i32 },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_RG   | SR_BYTE,           .fun = nearest_i16 },

        { .key = TEXTURE_MASK | SR_NEAREST | SR_RGB  | SR_UNSIGNED_BYTE,  .fun = nearest_u32 },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_BGR  | SR_UNSIGNED_BYTE,  .fun = nearest_u32 },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_RGBA | SR_UNSIGNED_BYTE,  .fun = nearest_u32 },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_BGRA | SR_UNSIGNED_BYTE,  .fun = nearest_u32 },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_RG   | SR_UNSIGNED_BYTE,  .fun = nearest_u16 },

        { .key = TEXTURE_MASK | SR_NEAREST | SR_RED  | SR_BYTE,           .fun = nearest_i8  },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_RED  | SR_UNSIGNED_BYTE,  .fun = nearest_u8  },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_RED  | SR_SHORT,          .fun = nearest_i16 },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_RED  | SR_UNSIGNED_SHORT, .fun = nearest_u16 },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_RED  | SR_INT,            .fun = nearest_i32 },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_RED  | SR_UNSIGNED_INT,   .fun = nearest_u32 },
        { .key = TEXTURE_MASK | SR_NEAREST | SR_RED  | SR_FLOAT,          .fun = nearest_f32 },

        // function pointer to linear interpolation in 1D
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_RGB  | SR_BYTE,           .fun = linear_i32 },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_BGR  | SR_BYTE,           .fun = linear_i32 },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_RGBA | SR_BYTE,           .fun = linear_i32 },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_BGRA | SR_BYTE,           .fun = linear_i32 },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_RG   | SR_BYTE,           .fun = linear_i16 },

        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_RGB  | SR_UNSIGNED_BYTE,  .fun = linear_u32 },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_BGR  | SR_UNSIGNED_BYTE,  .fun = linear_u32 },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_RGBA | SR_UNSIGNED_BYTE,  .fun = linear_u32 },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_BGRA | SR_UNSIGNED_BYTE,  .fun = linear_u32 },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_RG   | SR_UNSIGNED_BYTE,  .fun = linear_u16 },

        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_RED  | SR_BYTE,           .fun = linear_i8  },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_RED  | SR_UNSIGNED_BYTE,  .fun = linear_u8  },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_RED  | SR_SHORT,          .fun = linear_i16 },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_RED  | SR_UNSIGNED_SHORT, .fun = linear_u16 },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_RED  | SR_INT,            .fun = linear_i32 },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_RED  | SR_UNSIGNED_INT,   .fun = linear_u32 },
        { .key = SR_TEXTURE_1D | SR_LINEAR | SR_RED  | SR_FLOAT,          .fun = linear_f32 },

        // function pointer to linear interpolation in 2D
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_RGB  | SR_BYTE,           .fun = bilinear_i32 },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_BGR  | SR_BYTE,           .fun = bilinear_i32 },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_RGBA | SR_BYTE,           .fun = bilinear_i32 },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_BGRA | SR_BYTE,           .fun = bilinear_i32 },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_RG   | SR_BYTE,           .fun = bilinear_i16 },

        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_RGB  | SR_UNSIGNED_BYTE,  .fun = bilinear_u32 },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_BGR  | SR_UNSIGNED_BYTE,  .fun = bilinear_u32 },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_RGBA | SR_UNSIGNED_BYTE,  .fun = bilinear_u32 },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_BGRA | SR_UNSIGNED_BYTE,  .fun = bilinear_u32 },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_RG   | SR_UNSIGNED_BYTE,  .fun = bilinear_u16 },

        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_RED  | SR_BYTE,           .fun = bilinear_i8  },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_RED  | SR_UNSIGNED_BYTE,  .fun = bilinear_u8  },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_RED  | SR_SHORT,          .fun = bilinear_i16 },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_RED  | SR_UNSIGNED_SHORT, .fun = bilinear_u16 },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_RED  | SR_INT,            .fun = bilinear_i32 },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_RED  | SR_UNSIGNED_INT,   .fun = bilinear_u32 },
        { .key = TEXTURE_MASK_2D | SR_LINEAR | SR_RED  | SR_FLOAT,          .fun = bilinear_f32 },
    };

    MAP_INIT(map, arr);
}



