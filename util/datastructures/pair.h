//
// Created by Luis Ruisinger on 26.12.24.
//

#ifndef SOFTWARE_RATYTRACING_PAIR_H
#define SOFTWARE_RATYTRACING_PAIR_H

#include "../defines.h"

#define PAIR(type_0, type_1) \
    struct { type_0 p0; type_1 p1; }

#define FIRST(pair) \
    ((pair).p0)

#define SECOND(pair) \
    ((pair).p1)

#endif //SOFTWARE_RATYTRACING_PAIR_H
