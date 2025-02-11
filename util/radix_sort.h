#ifndef CPU_RAYTRACING_RADIX_SORT_H
#define CPU_RAYTRACING_RADIX_SORT_H

#include "defines.h"

void radix_sort_parallel(
    u8 *__restrict keys, u8 *__restrict keys_cpy,
    u8 *__restrict values, u8 *__restrict values_cpy, 
    usize len, usize bit_cnt);

#endif //CPU_RAYTRACING_RADIX_SORT_H