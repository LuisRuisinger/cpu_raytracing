//
// Created by Luis Ruisinger on 26.12.24.
//

#ifndef SOFTWARE_RATYTRACING_KEY_EVENT_HANDLER_H
#define SOFTWARE_RATYTRACING_KEY_EVENT_HANDLER_H

#include "util/defines.h"

typedef struct State_t State;
typedef struct Function_t {
    void (* fun)(State *);
} Function;

typedef enum KeyEvent_t {
    PRESSED, RELEASED, REPEAT
} KeyEvent;

void key_event_handler_init(void);
void lock_key(u32 key, State *state);
void release_key(u8 key, State *state);
void handle_keys(State *state);
void register_action(KeyEvent event, u32 key, Function fun);

typedef struct KeyEventHandler_t KeyEventHandler;
extern KeyEventHandler key_event_handler;

#endif //SOFTWARE_RATYTRACING_KEY_EVENT_HANDLER_H
