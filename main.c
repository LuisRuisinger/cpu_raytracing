#include <time.h>
#include <SDL.h>
#include <SDL_timer.h>
#include <assert.h>

#include "util/camera.h"
#include "util/defines.h"
#include "key_event_handler.h"
#include "util/datastructures/dynamic_array.h"
#include "util/datastructures/pair.h"
#include "util/fmt.h"
#include "util/vec3.h"
#include "model/obj_parser.h"

#define DEFAULT_WIDTH  680
#define DEFAULT_HEIGHT 480

typedef struct SDL_WindowHandle_t {
    SDL_Window  *window;
    SDL_Surface *surface;

    u32 width;
    u32 height;

    u32 tick;
} SDL_WindowHandle;

SDL_WindowHandle global_state;

typedef struct State_t {
    Camera *camera;
    f32 deltatime;
} State;

int init(u32 width, u32 height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *window =
            SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DEFAULT_WIDTH, DEFAULT_HEIGHT, 0);

    if (!window) {
        fprintf(stderr, "Error initializing window: %s\n", SDL_GetError());

        SDL_Quit();
        return -1;
    }

    SDL_Surface *surface = SDL_GetWindowSurface(window);

    if (!surface) {
        fprintf(stderr, "Error initializing surface: %s\n", SDL_GetError());

        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    global_state = (SDL_WindowHandle) {
            .window  = window,
            .surface = surface,
            .width   = width,
            .height  = height
    };

    return 0;
}

// TODO: fuck this - I haven't implemented C "sort of" lambdas YET
// an alternative could be the GCC extension but this could heavily restrict us
void move_forward (State *state) { move(state->camera, FORWARD, state->deltatime); }
void move_backward(State *state) { move(state->camera, BACKWARD, state->deltatime); }
void move_left    (State *state) { move(state->camera, LEFT, state->deltatime); }
void move_right   (State *state) { move(state->camera, RIGHT, state->deltatime); }
void move_up      (State *state) { move(state->camera, UP, state->deltatime); }
void move_down    (State *state) { move(state->camera, DOWN, state->deltatime); }

i32 polygon_sort(const void *a, const void *b) {
    const Triangle *_a = *(const Triangle **) a;
    const Triangle *_b = *(const Triangle **) b;

    f32 a_len = SIGN(_a->centroid) * length(_a->centroid);
    f32 b_len = SIGN(_b->centroid) * length(_b->centroid);

    if (a_len < b_len) {
        return -1;
    }

    if (a_len > b_len) {
        return 1;
    }

    return 0;
}

int main(void) {
    init(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    key_event_handler_init();
    log_handler_init();

    State state = {
            .deltatime = 1.0F,
            .camera = camera_create()
    };

    register_action(REPEAT, SDLK_w, (Function) { move_forward });
    register_action(REPEAT, SDLK_a, (Function) { move_left });
    register_action(REPEAT, SDLK_s, (Function) { move_backward });
    register_action(REPEAT, SDLK_d, (Function) { move_right });
    register_action(REPEAT, SDLK_SPACE, (Function) { move_up });
    register_action(REPEAT, SDLK_c, (Function) { move_down });


    TriangleArr arr;
    if (parse("../model/test/cube.obj", &arr) == -1) {
        LOG("Obj file parsing failed");
        LOG("Terminating ...");
        exit(EXIT_FAILURE);
    }

    /* TODO */
    ARRAY_SORT(arr, polygon_sort);
    Triangle *entry = NULL;
    ARRAY_FOREACH(arr, entry) {
        LOG("%.2f %.2f %.2f", GET_VEC3_X(entry->centroid), GET_VEC3_Y(entry->centroid), GET_VEC3_Z(entry->centroid));
    }
    exit(-1);

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
                        lock_key(event.key.keysym.sym, &state);
                    }

                    break;
                }
                case SDL_KEYUP: {
                    release_key(event.key.keysym.sym, &state);
                    break;
                }
                case SDL_MOUSEMOTION: {
                    rotate(state.camera, (f32) event.motion.x, (f32) event.motion.y);
                    break;
                }
            }
        }

        handle_keys(&state);
        camera_stringify(state.camera);
        SDL_UpdateWindowSurface(global_state.window);
    }

    SDL_DestroyWindow(global_state.window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
