#include <time.h>
#include <SDL.h>
#include <SDL_timer.h>
#include <assert.h>

#include "util/camera.h"
#include "util/defines.h"
#include "util/fmt.h"

#include "key_event_handler.h"
#include "bvh/bvh.h"

typedef struct State_t {
    SDL_Window  *window;
    SDL_Surface *surface;

    u32 width;
    u32 height;

    Camera *camera;
    f32 deltatime;

    u32 tick;
} State;

State global_state;

void move_forward (State *state) { move(state->camera, FORWARD,  state->deltatime); }
void move_backward(State *state) { move(state->camera, BACKWARD, state->deltatime); }
void move_left    (State *state) { move(state->camera, LEFT,     state->deltatime); }
void move_right   (State *state) { move(state->camera, RIGHT,    state->deltatime); }
void move_up      (State *state) { move(state->camera, UP,       state->deltatime); }
void move_down    (State *state) { move(state->camera, DOWN,     state->deltatime); }

i32 init(u32 width, u32 height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
        LOG("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow(
            NULL,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DEFAULT_WIDTH, DEFAULT_HEIGHT, 0);

    if (!window) {
        LOG("Error initializing window: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Surface *surface = SDL_GetWindowSurface(window);

    if (!surface) {
        LOG("Error initializing surface: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    key_event_handler_init();
    log_handler_init();

    register_action(REPEAT, SDLK_w, (Function) { move_forward });
    register_action(REPEAT, SDLK_a, (Function) { move_left });
    register_action(REPEAT, SDLK_s, (Function) { move_backward });
    register_action(REPEAT, SDLK_d, (Function) { move_right });
    register_action(REPEAT, SDLK_SPACE, (Function) { move_up });
    register_action(REPEAT, SDLK_c, (Function) { move_down });

    global_state = (State) {
            .window  = window,
            .surface = surface,
            .width   = width,
            .height  = height,
            .deltatime = 1.0F,
            .camera = camera_create()
    };

    return 0;
}

int main(void) {
    init(DEFAULT_WIDTH, DEFAULT_HEIGHT);

    /* TODO */
    // console loader for textures
    const char *file = "../model/test/spot/spot_quadrangulated.obj";
    bvh_build(&file, 1);

    // rendering
    clock_t timestamp;
    u32 *pixels = (u32 *) global_state.surface->pixels;

    bool running = true;
    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    running = false;
                    break;
                }
                case SDL_KEYDOWN: {
                    if (event.key.repeat == 0) {
                        lock_key(event.key.keysym.sym, &global_state);
                    }

                    break;
                }
                case SDL_KEYUP: {
                    release_key(event.key.keysym.sym, &global_state);
                    break;
                }
                case SDL_MOUSEMOTION: {
                    rotate(global_state.camera, (f32) event.motion.x, (f32) event.motion.y);
                    break;
                }
            }
        }

        handle_keys(&global_state);
        camera_stringify(global_state.camera);
        SDL_UpdateWindowSurface(global_state.window);
    }

    SDL_DestroyWindow(global_state.window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

