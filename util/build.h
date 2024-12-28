//
// Created by Luis Ruisinger on 27.12.24.
//

#ifndef CPU_RAYTRACING_BUILD_H
#define CPU_RAYTRACING_BUILD_H

// capturing C++ version
#if __cplusplus >= 199711L
    #define CPU_RAYTRACING_CPP98_LEAST
#endif
#if __cplusplus >= 201103L
    #define CPU_RAYTRACING_CPP11_LEAST
#endif
#if __cplusplus >= 201402L
    #define CPU_RAYTRACING_CPP14_LEAST
#endif
#if __cplusplus >= 201703L
    #define CPU_RAYTRACING_CPP17_LEAST
#endif
#if __cplusplus >= 202002L
    #define CPU_RAYTRACING_CPP20_LEAST
#endif

#ifdef __cplusplus
    #define C_GUARD_BEGINN() extern "C" {
    #define C_GUARD_END()    }
#else
    #define C_GUARD_BEGINN()
    #define C_GUARD_END()
#endif

// capturing C version
#ifdef __STDC__
    #define CPU_RAYTRACING_C89_LEAST
#endif
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199409L
    #define CPU_RAYTRACING_C94_LEAST
#endif
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #define CPU_RAYTRACING_C99_LEAST
#endif
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define CPU_RAYTRACING_C11_LEAST
#endif
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201710L
    #define CPU_RAYTRACING_C17_LEAST
#endif
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    #define CPU_RAYTRACING_C23_LEAST
#endif

// compiler
#if defined(__GNUC__) && !defined(__clang__)
    #define CPU_RAYTRACING_GNUC __GNUC__
    #define ALWAYS_INLINE __attribute__((always_inline))
    #define HAS_TYPEOF
#elif defined(__GNUC__)
    #define CPU_RAYTRACING_GNUC_CLANG
    #define ALWAYS_INLINE __attribute__((always_inline))
    #define HAS_TYPEOF
#elif defined(__clang__)
    #define CPU_RAYTRACING_CLANG __clang_major__
    #define ALWAYS_INLINE __attribute__((always_inline))
    #define HAS_TYPEOF
#elif defined(_MSC_VER)
    #define CPU_RAYTRACING_MSC _MSC_VER
    #define ALWAYS_INLINE __forceinline
#else
    #define ALWAYS_INLINE inline
#endif

// architecture
#if defined(__x86_64__)
    #define CPU_RAYTRACING_X86
#endif

#endif //CPU_RAYTRACING_BUILD_H
