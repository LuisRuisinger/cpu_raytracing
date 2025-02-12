#ifndef SOFTWARE_RATYTRACING_TEXTURE_H
#define SOFTWARE_RATYTRACING_TEXTURE_H

#include "defines.h"
#include "linalg/vec3.h"
#include "linalg/matrix.h"
#include "datastructures/dict.h"

C_GUARD_BEGINN()

enum {

    // textures
    SR_TEXTURE_1D,
	SR_TEXTURE_2D,
	SR_TEXTURE_3D,
	SR_TEXTURE_1D_ARRAY,
	SR_TEXTURE_2D_ARRAY,
    
    //texture parameters i
	SR_TEXTURE_BASE_LEVEL,
	SR_TEXTURE_COMPARE_FUNC,
	SR_TEXTURE_COMPARE_MODE,
	SR_TEXTURE_LOD_BIAS,
	SR_TEXTURE_MIN_FILTER,
	SR_TEXTURE_MAG_FILTER,
	SR_TEXTURE_MIN_LOD,
	SR_TEXTURE_MAX_LOD,
	SR_TEXTURE_MAX_LEVEL,
	SR_TEXTURE_SWIZZLE_R,
	SR_TEXTURE_SWIZZLE_G,
	SR_TEXTURE_SWIZZLE_B,
	SR_TEXTURE_SWIZZLE_A,
	SR_TEXTURE_SWIZZLE_RGBA,
	SR_TEXTURE_WRAP_S,
	SR_TEXTURE_WRAP_T,
	SR_TEXTURE_WRAP_R,

    //texture parameter values
	SR_REPEAT,
	SR_CLAMP_TO_EDGE,
	SR_MIRRORED_REPEAT,
	SR_NEAREST,
	SR_LINEAR,

    /* TODO */
    // implement mipmaping
	// SR_NEAREST_MIPMAP_NEAREST,
	// SR_NEAREST_MIPMAP_LINEAR,
	// SR_LINEAR_MIPMAP_NEAREST,
	// SR_LINEAR_MIPMAP_LINEAR,

    // format
	// we need to mask the format into the other i32 identifier 
	// to resolve function pointer at runtime
    SR_RED,
	SR_RG,
	SR_RGB,
	SR_BGR,
	SR_RGBA,
	SR_BGRA,

	/* TODO */
	// support
	// SR_COMPRESSED_RED,
	// SR_COMPRESSED_RG,
	// SR_COMPRESSED_RGB,
	// SR_COMPRESSED_RGBA,

    // data types
    // only allow data packable in 32 bits 
    // interpretation of the 32 bit unsigned value gets deligated to the caller
	SR_UNSIGNED_BYTE,
	SR_BYTE,
	SR_UNSIGNED_SHORT,
	SR_SHORT,
	SR_UNSIGNED_INT,
	SR_INT,
	SR_FLOAT,
};

u32 texture_sample(u32 tex_id, vec3f v);
void texture_setup(void);

C_GUARD_END()

#endif //SOFTWARE_RATYTRACING_TEXTURE_H