//
// Created by Luis Ruisinger on 06.01.25.
//

#include "vec2.h"

vec2f vec2_add(vec2f a, vec2f b) {
    return (vec2f) {
            .x = a.x + b.x,
            .y = a.y + b.y
    };
}

vec2f vec2_adds(vec2f a, f32 scalar) {
    return (vec2f) {
            .x = a.x + scalar,
            .y = a.y + scalar
    };
}

vec2f vec2_sub(vec2f a, vec2f b) {
    return (vec2f) {
            .x = a.x - b.x,
            .y = a.y - b.y
    };
}

vec2f vec2_subs(vec2f a, f32 scalar) {
    return (vec2f) {
            .x = a.x - scalar,
            .y = a.y - scalar
    };
}

vec2f vec2_mul(vec2f a, vec2f b) {
    return (vec2f) {
            .x = a.x * b.x,
            .y = a.y * b.y
    };
}

vec2f vec2_muls(vec2f a, f32 scalar) {
    return (vec2f) {
            .x = a.x * scalar,
            .y = a.y * scalar
    };
}

vec2f vec2_divs(vec2f a, f32 scalar) {
    return (vec2f) {
            .x = a.x / scalar,
            .y = a.y / scalar
    };
}

f32 vec2_length(vec2f a) {
    return sqrtf(a.x * a.x + a.y * a.y);
}

vec2f vec2_normalize(vec2f a) {
    return vec2_muls(a, vec2_length(a));
}

f32 vec2_dot(vec2f a, vec2f b) {
    return a.x * b.x + a.y + b.y;
}

vec2f vec2_inverse(vec2f a) {
    return (vec2f) {
            a.x != 0 ? 1.0F / a.x : INFINITY,
            a.y != 0 ? 1.0F / a.y : INFINITY,
    };
}

vec2f vec2_min(vec2f a, vec2f b) {
    return (vec2f) {
            .x = a.x < b.x ? a.x : b.x,
            .y = a.y < b.y ? a.y : b.y,
    };
}

vec2f vec2_max(vec2f a, vec2f b) {
    return (vec2f) {
            .x = a.x > b.x ? a.x : b.x,
            .y = a.y > b.y ? a.y : b.y,
    };
}

vec2f vec2_refract(vec2f i, vec2f n, f32 eta) {
    f32 tmp_0 = vec2_dot(n, i);
    f32 k = 1.0F - eta * eta * (1.0F - tmp_0 * tmp_0);

    if (k < 0.0F) {
        return VEC2(0.0F);
    }

    f32 tmp_1 = eta * tmp_0 + sqrtf(k);
    n = vec2_muls(n, tmp_1);
    i = vec2_muls(i, eta);

    return vec2_sub(i, n);
}

vec2f rec2_reflect(vec2f i, vec2f n) {
    f32 tmp_0 = 2.0F * vec2_dot(n, i);
    n = vec2_muls(n, tmp_0);

    return vec2_sub(i, n);
}