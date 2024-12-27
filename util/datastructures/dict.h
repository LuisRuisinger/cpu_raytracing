//
// Created by Luis Ruisinger on 17.12.24.
//

#ifndef SOFTWARE_RATYTRACING_DICT_H
#define SOFTWARE_RATYTRACING_DICT_H

#include "dynamic_array.h"
#include "../defines.h"

C_GUARD_BEGINN()

#define ENTRY(key_type, value_type) \
    struct { key_type key; value_type value; }

#define ENTRY_FUNC(key_type, value_type) \
    struct { key_type key; value_type; }

#define MAP(type_0, type_1)                                                                       \
    struct {                                                                                      \
        ARRAY(ENTRY(type_0, type_1)) entries;                                                     \
        i32 (*cmp_fun) (type_0 *key_1, type_0 *key_2);                                            \
    }

#define MAP_FUNC(type_0, type_1)                                                                       \
    struct {                                                                                      \
        ARRAY(ENTRY_FUNC(type_0, type_1)) entries;                                                     \
        i32 (*cmp_fun) (type_0 *key_1, type_0 *key_2);                                            \
    }

#define MAP_INIT(map, v) \
    do { ARRAY_APPEND((map).entries, v); } while (0)

#define CONTAINS(map, key_0, res)                                                                 \
    do {                                                                                          \
        *(res) = false;                                                                           \
        __typeof__(ARRAY_ELEMENT((token_map).entries, 0)) *entry = NULL;                          \
        ARRAY_FOREACH((map).entries, entry) {                                                     \
            if ((map).cmp_fun(key_0, (entry)->key) == 0) {                                        \
                *(res) = true;                                                                    \
                break;                                                                            \
            }                                                                                     \
        }                                                                                         \
    }                                                                                             \
    while (0)

#define INSERT(map, entry, res)                                                                   \
    do {                                                                                          \
        *res = false;                                                                             \
        bool exists;                                                                              \
        CONTAINS(map, (entry).key, &exists);                                                      \
                                                                                                  \
        if (!exists) {                                                                            \
            ARRAY_PUSH_BACK((map).entries, entry);                                                \
            *res = true;                                                                          \
        }                                                                                         \
    }                                                                                             \
    while (0)

#define GET(map, key_0, res)                                                                      \
    do {                                                                                          \
        __typeof__(ARRAY_ELEMENT((map).entries, 0)) *entry = NULL;                                 \
        ARRAY_FOREACH((map).entries, entry) {                                                     \
            if ((map).cmp_fun((char **) key_0, &(entry)->key) == 0) {                             \
                *(res) = *entry;                                                                   \
                break;                                                                            \
            }                                                                                     \
        }                                                                                         \
    }                                                                                             \
    while (0)

C_GUARD_END()

#endif //SOFTWARE_RATYTRACING_DICT_H
