//
// Created by Luis Ruisinger on 26.12.24.
//

#include "key_event_handler.h"
#include "util/datastructures/dynamic_array.h"
#include "util/datastructures/pair.h"
#include "util/datastructures/bitmap.h"

typedef struct KeyEventHandler_t {
    BITMAP(bitmap, 128);

    ARRAY(PAIR(u32, Function)) on_pre;
    ARRAY(PAIR(u32, Function)) on_rel;
    ARRAY(PAIR(u32, Function)) on_rep;
} KeyEventHandler;

KeyEventHandler key_event_handler;

void key_event_handler_init(void) {
    bitmap_zero(key_event_handler.bitmap, 128);

    ARRAY_INIT(key_event_handler.on_pre, 32);
    ARRAY_INIT(key_event_handler.on_rep, 32);
    ARRAY_INIT(key_event_handler.on_rel, 32);
}

void lock_key(u32 key, State *state) {
    ARRAY_TYPEOF(key_event_handler.on_pre) *pair = NULL;
    ARRAY_FOREACH(key_event_handler.on_pre, pair) {
        if (FIRST(*pair) == key) {
            SECOND(*pair).fun(state);
            break;
        }
    }

    bitmap_set_bit(key_event_handler.bitmap, key);
}

void release_key(u8 key, State *state) {
    ARRAY_TYPEOF(key_event_handler.on_rel) *pair = NULL;
    ARRAY_FOREACH(key_event_handler.on_rel, pair) {
        if (FIRST(*pair) == key) {
            SECOND(*pair).fun(state);
            break;
        }
    }

    bitmap_clear_bit(key_event_handler.bitmap, key);
}

void handle_keys(State *state) {
    ARRAY_TYPEOF(key_event_handler.on_rep) *pair = NULL;
    ARRAY_FOREACH(key_event_handler.on_rep, pair) {
        u32 key = FIRST(*pair);

        if (bitmap_test_bit(key_event_handler.bitmap, key)) {
            SECOND(*pair).fun(state);
        }
    }
}

void register_action(KeyEvent event, u32 key, Function fun) {
    switch (event) {
        case PRESSED: {
            ARRAY_TYPEOF(key_event_handler.on_pre) pair = { key, fun};
            ARRAY_PUSH_BACK(key_event_handler.on_pre, pair);
            break;
        }

        case RELEASED: {
            ARRAY_TYPEOF(key_event_handler.on_rel) pair = { key, fun};
            ARRAY_PUSH_BACK(key_event_handler.on_rel, pair);
            break;
        }

        case REPEAT: {
            ARRAY_TYPEOF(key_event_handler.on_rep) pair = { key, fun};
            ARRAY_PUSH_BACK(key_event_handler.on_rep, pair);
            break;
        }
    }
}

KeyEventHandler key_event_handler;
