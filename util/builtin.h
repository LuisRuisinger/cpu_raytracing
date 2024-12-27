//
// Created by Luis Ruisinger on 27.12.24.
//

#ifndef CPU_RAYTRACING_BUILTIN_H
#define CPU_RAYTRACING_BUILTIN_H

#include <stdbool.h>
#include <stdio.h>

#include "build.h"

C_GUARD_BEGINN()

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef float_t  f32;
typedef double_t f64;

typedef size_t   usize;
typedef ssize_t  ssize;

#if defined(CPU_RAYTRACING_X86)
#if defined(__AVX2__) || defined(__AVX__)
    #include <immintrin.h>
    #define CPU_RAYTRACING_AVX2
    #define ALIGNMENT_256 sizeof(__m256)
#endif

#if defined(__SSE4_2__)
    #include <nmmintrin.h>
    #define CPU_RAYTRACING_SSE4_2
#endif

#if defined(__SSE4_1__)
    #include <smmintrin.h>
    #define CPU_RAYTRACING_SSE4_2
#endif

#if defined(__SSSE3__)
    #include <tmmintrin.h>
    #define CPU_RAYTRACING_SSSE3
#endif

#if defined(__SSE3__)
    #include <pmmintrin.h>
    #define CPU_RAYTRACING_SSE3
#endif

#if defined(__SSE2__)
    #include <emmintrin.h>
    #define CPU_RAYTRACING_SSE2
    #define ALIGNMENT_128 sizeof(__m128)
#endif
#endif

#if defined(__has_builtin)
    #define CPU_RAYTRACING_HAS_BUILTIN(intrin) __has_builtin(intrin)
#elif
    #define CPU_RAYTRACING_HAS_BUILTIN(intrin) true
#endif

// unreachable
#if defined(CPU_RAYTRACING_GNUC_CLANG) && CPU_RAYTRACING_HAS_BUILTIN(__builtin_unreachable)
    #define CPU_RAYTRACING_UNREACHABLE() __builtin_unreachable()
#elif defined(CPU_RAYTRACING_MSC)
    #define CPU_RAYTRACING_UNREACHABLE(stmt) __assume(false)
#else
    #define CPU_RAYTRACING_UNREACHABLE()
#endif

// assume
#if defined(CPU_RAYTRACING_GNUC_CLANG) && CPU_RAYTRACING_HAS_BUILTIN(__builtin_assume)
    #define CPU_RAYTRACING_ASSUME(stmt) __builtin_assume(stmt)
#elif defined(CPU_RAYTRACING_MSC)
    #define CPU_RAYTRACING_ASSUME(stmt) __assume(stmt)
#else
    #define CPU_RAYTRACING_ASSUME(stmt)
#endif

#if defined(CPU_RAYTRACING_GNUC_CLANG) && \
    CPU_RAYTRACING_HAS_BUILTIN(__builtin_clz) && CPU_RAYTRACING_HAS_BUILTIN(__builtin_clzll)

    ALWAYS_INLINE static i32 cr_clz_u8(u8 x) {
        return __builtin_clz(x) - (i32) ((sizeof(u32) - sizeof(u8)) * 8);
    }

    ALWAYS_INLINE static i32 cr_clz_u16(u16 x) {
        return __builtin_clz(x) - (i32) ((sizeof(u32) - sizeof(u16)) * 8);
    }

    ALWAYS_INLINE static i32 cr_clz_u32(u32 x) {
        return __builtin_clz(x);
    }

    ALWAYS_INLINE static i32 cr_clz_u64(u64 x) {
        return __builtin_clzll(x);
    }
#else
    // src https://stackoverflow.com/a/23862121
    ALWAYS_INLINE static i32 clz(u32 x) {
        static const u8 debruijn_32[32] = {
                0, 31, 9, 30, 3, 8, 13, 29, 2, 5, 7, 21, 12, 24, 28, 19,
                1, 10, 4, 14, 6, 22, 25, 20, 11, 15, 23, 26, 16, 27, 17, 18
        };

        // normally 0 is not being supported
        if (!x) {
            return 32;
        }

        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        x++;

        return debruijn_32[x * 0x076be629 >> 27];
    }

    ALWAYS_INLINE static i32 cr_clz_u8(u8 x) {
        return clz(x) - (i32) ((sizeof(u32) - sizeof(u8)) * 8);
    }

    ALWAYS_INLINE static i32 cr_clz_u16(u16 x) {
        return clz(x) - (i32) ((sizeof(u32) - sizeof(u16)) * 8);
    }

    ALWAYS_INLINE static i32 cr_clz_u32(u32 x) {
        return clz(x);
    }

    ALWAYS_INLINE static i32 cr_clz_u64(u64 x) {
        i32 upper = clz(x >> 32);
        return upper ? upper : clz((u32) x);
    }
#endif

// TODO: add support for other compilers ?

#if defined(CPU_RAYTRACING_C11_LEAST)
    #define CPU_RAYTRACING_CLZ(x) \
        _Generic((x), u8: cr_clz_u8, u16: cr_clz_u16, u32: cr_clz_u32, u64: cr_clz_u64)(x)
#endif

C_GUARD_END()

#endif //CPU_RAYTRACING_BUILTIN_H
