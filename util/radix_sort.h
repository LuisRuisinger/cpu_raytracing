#ifndef CPU_RAYTRACING_RADIX_SORT_H
#define CPU_RAYTRACING_RADIX_SORT_H

#include <string.h>

#include "defines.h"

#define RADIX_MASK \
    ((1ULL << BYTE_SIZE) - 1)

#define RADIX_BUFFER_SIZE \
    (1ULL << BYTE_SIZE)

#define RADIX_SORT_IMPL(_type) \
    static void radix_sort_##_type(_type *__restrict src, _type *__restrict cpy, usize len) {       \
        _type *__restrict _src = src;                                                               \
        _type *__restrict _cpy = cpy;                                                               \
                                                                                                    \
        usize idx[RADIX_BUFFER_SIZE];                                                               \
        for (usize j = 0; j < sizeof(_type); ++j) {                                                 \
            memset(idx, 0, sizeof(idx));                                                            \
                                                                                                    \
            usize shift = BYTE_SIZE * j;                                                            \
            usize next_index = 0;                                                                   \
            usize cnt = 0;                                                                          \
                                                                                                    \
            for (usize i = 0; i < len; ++i) {                                                       \
                ++idx[(_src[i] >> shift) & RADIX_MASK];                                             \
            }                                                                                       \
                                                                                                    \
            for (usize i = 0; i < RADIX_BUFFER_SIZE; ++i){                                          \
                cnt = idx[i];                                                                       \
                idx[i] = next_index;                                                                \
                next_index += cnt;                                                                  \
            }                                                                                       \
                                                                                                    \
            for (usize i = 0; i < len; ++i) {                                                       \
                usize _idx = idx[(_src[i] >> shift) & RADIX_MASK];                                  \
                _cpy[_idx] = _src[i];                                                               \  
                                                                                                    \
                ++idx[(_src[i] >> shift) & RADIX_MASK];                                             \                                                               
            }                                                                                       \
                                                                                                    \
            _src = (_type *) ((usize) _src ^ (usize) _cpy);                                         \
            _cpy = (_type *) ((usize) _src ^ (usize) _cpy);                                         \
            _src = (_type *) ((usize) _src ^ (usize) _cpy);                                         \
        }                                                                                           \
    }

RADIX_SORT_IMPL(u64)

#define RADIX_SORT(_type, ...) \
    radix_sort_##_type(__VA_ARGS__)


void radix_sort_parallel(u64 *__restrict src, u64 *__restrict cpy, usize len);

#endif //CPU_RAYTRACING_RADIX_SORT_H