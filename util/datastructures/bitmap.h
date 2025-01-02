//
// Created by Luis Ruisinger on 28.12.24.
//

#ifndef CPU_RAYTRACING_BITMAP_H
#define CPU_RAYTRACING_BITMAP_H

#include <stdlib.h>
#include <string.h>

#include "../defines.h"

C_GUARD_BEGINN()

// machine word size
// afaik on 32 bit machines 32bit and on 64 bit machines 64 bit
// and at least 32 bit long
typedef unsigned long BITMAP_DWORD;

#define BITMAP_DWORD_SIZE (sizeof(BITMAP_DWORD) * 8)

// amount of dwords needed to represent the cnt bits in a bitmap
#define BITMAP_N_DWORDS(cnt) \
    (((cnt) + BITMAP_DWORD_SIZE - 1) / BITMAP_DWORD_SIZE)

typedef struct Bitmap_t {
    BITMAP_DWORD dword;
} Bitmap;

#define BITMAP(name, cnt) \
    Bitmap (name)[BITMAP_N_DWORDS(cnt)]

#define BITMAP_DWORD(bitmap, bit) \
    ((bitmap)[(bit) / BITMAP_DWORD_SIZE].dword)

// this assumes little-endian ordering
// TODO: endian awareness
#define BITMAP_DWORD_BIT(bit) \
    (1UL << ((bit) & (BITMAP_DWORD_SIZE - 1)))

ALWAYS_INLINE static usize bitmap_sizeof(u32 cnt) {
    return BITMAP_N_DWORDS(cnt) * sizeof(BITMAP_DWORD);
}

ALWAYS_INLINE static void bitmap_set_bit(Bitmap *bitmap, u32 n) {
    BITMAP_DWORD(bitmap, n) |= BITMAP_DWORD_BIT(n);
}

ALWAYS_INLINE static void bitmap_set_bits(Bitmap *bitmap, u32 n, u32 cnt) {
    for (usize i = 0; i < cnt; ++i) {
        BITMAP_DWORD(bitmap, n + i) |= BITMAP_DWORD_BIT(n + i);
    }
}

ALWAYS_INLINE static void bitmap_clear_bit(Bitmap *bitmap, u32 n) {
    BITMAP_DWORD(bitmap, n) &= ~BITMAP_DWORD_BIT(n);
}

ALWAYS_INLINE static void bitmap_flip_bit(Bitmap *bitmap, u32 n) {
    BITMAP_DWORD(bitmap, n) ^= BITMAP_DWORD_BIT(n);
}

ALWAYS_INLINE static bool bitmap_test_bit(Bitmap *bitmap, u32 n) {
    return (bool) (BITMAP_DWORD(bitmap, n) & BITMAP_DWORD_BIT(n));
}

ALWAYS_INLINE static void bitmap_zero(Bitmap *bitmap, u32 cnt) {
    memset(bitmap, 0, bitmap_sizeof(cnt));
}

ALWAYS_INLINE static void bitmap_fill(Bitmap *bitmap, u32 cnt) {
    memset(bitmap, UINT8_MAX, bitmap_sizeof(cnt));
}

ALWAYS_INLINE static void bitmap_cpy(Bitmap *dst, Bitmap *src, u32 cnt) {
    memcpy(dst, src, bitmap_sizeof(cnt));
}

ALWAYS_INLINE static void bitmap_complement(Bitmap *dst, Bitmap *src, u32 cnt) {
    for (usize i = 0; i < BITMAP_N_DWORDS(cnt); ++i) {
            dst[i].dword = ~src[i].dword;
    }
}

ALWAYS_INLINE static u32 bitmap_popcount(Bitmap *bitmap, u32 cnt) {
    u32 popcnt = 0;

    // TODO: abstract popcount (no guarantee this builtin exists)
    for (usize i = 0; i < BITMAP_N_DWORDS(cnt); ++i) {
        popcnt += __builtin_popcount(bitmap[i].dword);
    }

    return popcnt;
}

#define BITMAP_DEFINE_BINOP(name, op)                                                             \
    ALWAYS_INLINE static void bitmap_##name(                                                      \
        Bitmap *dst, const Bitmap *a, const Bitmap *b, u32 cnt) {                                 \
        for (usize i = 0; i < BITMAP_N_DWORDS(cnt); ++i) {                                        \
            dst[i].dword = a[i].dword op b[i].dword;                                              \
        }                                                                                         \
    }

BITMAP_DEFINE_BINOP(and, &)
BITMAP_DEFINE_BINOP(nand, & ~)
BITMAP_DEFINE_BINOP(or, |)
BITMAP_DEFINE_BINOP(nor, | ~)
BITMAP_DEFINE_BINOP(xor, ^)
BITMAP_DEFINE_BINOP(nxor, ^ ~)

C_GUARD_END()

#endif //CPU_RAYTRACING_BITMAP_H
