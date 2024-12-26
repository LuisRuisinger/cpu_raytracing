//
// Created by Luis Ruisinger on 30.11.24.
//

#include <string.h>
#include "obj_parser.h"
#include "../util/datastructures/dynamic_array.h"
#include "../util/datastructures/dict.h"
#include "../util/vec3.h"

#define BASE_BUFFER (1024 * 4)
#define INVALID_DIGIT(c) ((c) < '0' || (c) > '9')

typedef enum OBJ_InternalState_t {
    INVALID = -1, END = 0, VALID = 1
} OBJ_InternalState;

typedef struct OBJ_State_t {

    // raw file obj file content
    const char *obj;

    // internal validation state
    OBJ_InternalState state;

    usize idx;
    usize len;

    // buffers
    ARRAY(vec3f) v_arr;
    ARRAY(vec3f) vn_arr;
    ARRAY(vec3f) vt_arr;
    ARRAY(Triangle) f_arr;
} OBJ_State;

usize obj_parse_v (OBJ_State *);
usize obj_parse_vn(OBJ_State *);
usize obj_parse_vt(OBJ_State *);
usize obj_parse_f (OBJ_State *);
usize obj_parse_sl(OBJ_State *);

static MAP_FUNC  (char *, usize (*parse)(OBJ_State *)) token_map;
static ENTRY_FUNC(char *, usize (*parse)(OBJ_State *)) entries[] = {

        // parsing the following token
        { "v",      obj_parse_v },
        { "vn",     obj_parse_vn },
        { "vt",     obj_parse_vt },
        { "f",      obj_parse_f },

        // skipping the following token
        { "#",      obj_parse_sl },
        { "s",      obj_parse_sl },
        { "o",      obj_parse_sl },
        { "#",      obj_parse_sl },
        { "mtllib", obj_parse_sl },
        { "usemtl", obj_parse_sl }
};

void init_parser(void) {
    static bool is_init = false;

    if (is_init) {
        return;
    }

    is_init = true;
    MAP_INIT(token_map, entries);
}

i32 parse(const char *file, ARRAY(Triangle) *res) {
    OBJ_State state = {
            .obj   = /* TODO */ NULL,
            .idx   = 0,
            .state = VALID
    };

    ARRAY_INIT(state.v_arr,  BASE_BUFFER);
    ARRAY_INIT(state.vn_arr, BASE_BUFFER);
    ARRAY_INIT(state.vt_arr, BASE_BUFFER);
    ARRAY_INIT(state.f_arr,  BASE_BUFFER);

    while (state.state == VALID) {
        usize (*p)(OBJ_State *) = NULL;
        __typeof__(ARRAY_ELEMENT((token_map).entries, 0)) visitor;
        GET(token_map, &state.obj[state.idx], &visitor);

        if (!visitor) {
            state.state = INVALID;
            break;
        }

        usize n_idx = visitor((OBJ_State *) &token_map);
        state.idx = n_idx;
    }

    ARRAY_MOVE(*res, state.f_arr);

    ARRAY_FREE(state.v_arr);
    ARRAY_FREE(state.vn_arr);
    ARRAY_FREE(state.vt_arr);
    ARRAY_FREE(state.f_arr);
    return state.state;
}

usize parse_floats(OBJ_State *obj_state, f32 *parsed, usize len) {
    usize i = obj_state->idx + 2;

    // validates space between token and remaining line
    if (obj_state->obj[i] != ' ') {
        obj_state->state = INVALID;
        return ++i;
    }

    for (usize j = 0; j < len && i < obj_state->len; ++j) {
        if (i + 1 == obj_state->len) {
            obj_state->state = INVALID;
            return ++i;
        }

        const char *begin = &obj_state->obj[i++];
        if (*begin != '-' && INVALID_DIGIT(*begin)) {
            obj_state->state = INVALID;
            return ++i;
        }

        for (; i < obj_state->len; ++i) {
            if (obj_state->obj[i] == ' ') {
                break;
            }

            if (INVALID_DIGIT(obj_state->obj[i])) {
                obj_state->state = INVALID;
                return ++i;
            }
        }

        parsed[j] = (f32) strtod(begin, NULL);
    }

    if (obj_state->obj[i] != '\n' && obj_state->obj[i]) {
        obj_state->state = INVALID;
        return ++i;
    }

    if (i + 1 == obj_state->len) {
        obj_state->state = END;
    }

    return ++i;
}

usize obj_parse_vt(OBJ_State *obj_state) {
    f32 parsed[2];

    usize n_i = parse_floats(obj_state, parsed, 2);

    vec3f tex = VEC3(parsed[0], parsed[1]);
    ARRAY_PUSH_BACK(obj_state->vt_arr, tex);

    return n_i;
}

usize obj_parse_vn(OBJ_State *obj_state) {
    f32 parsed[3];

    usize n_i = parse_floats(obj_state, parsed, 3);

    vec3f normal = VEC3(parsed[0], parsed[1], parsed[2]);
    ARRAY_PUSH_BACK(obj_state->vt_arr, normal);

    return n_i;
}

usize obj_parse_v(OBJ_State *obj_state) {
    f32 parsed[4];

    usize n_i = parse_floats(obj_state, parsed, 4);

    if (obj_state->state == INVALID) {
        if (obj_state->obj[n_i - 1] == '\n' || !obj_state->obj[n_i - 1]) {
            obj_state->state = VALID;
        }
        else {
            return n_i;
        }
    }

    vec3f vertex = VEC3(parsed[0], parsed[1], parsed[2]);
    ARRAY_PUSH_BACK(obj_state->vt_arr, vertex);

    return n_i;
}

usize obj_parse_f(OBJ_State *obj_state) {

}

usize obj_parse_sl(OBJ_State *obj_state) {
    usize i = obj_state->idx;

    // ignoring everything
    for (; i < obj_state->len; ++i) {
        if (obj_state->obj[i] == '\n') {
            break;
        }
    }

    if (!obj_state->obj[i]) {
        obj_state->state = END;
    }

    return ++i;
}



