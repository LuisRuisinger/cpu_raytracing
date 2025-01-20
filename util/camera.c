//
// Created by Luis Ruisinger on 25.12.24.
//

#include <assert.h>

#include "../util/camera.h"
#include "linalg/vec3.h"
#include "../util/fmt.h"

#define MAX_PITCH   89.0F
#define MIN_PITCH   (-89.0F)
#define YAW         0.0F
#define PITCH       0.0F
#define SPEED       32.0F
#define SENSITIVITY 0.1F

typedef struct Camera_t {

    // camera 3d world space attributes
    vec3f pos;
    vec3f front;
    vec3f up;
    vec3f right;
    vec3f world_up;

    // angles and 2d plane pos
    f32 yaw;
    f32 pitch;
    f32 last_xpos;
    f32 last_ypos;

    // options
    f32 ms_speed;
    f32 sensitivity;
} Camera;

static void camera_update(Camera *cam) {
    f32 cosf_yaw = cosf(TO_RADIANS(cam->yaw));
    f32 cosf_pitch = cosf(TO_RADIANS(cam->pitch));

    cam->front = VEC3(
            cosf_yaw * cosf_pitch,
            sinf(TO_RADIANS(cam->pitch)),
            sinf(TO_RADIANS(cam->yaw)) * cosf_pitch);
    cam->front = vec3_normalize(cam->front);

    cam->right = vec3_cross(cam->front, cam->world_up);
    cam->right = vec3_normalize(cam->right);

    cam->up = vec3_cross(cam->right, cam->front);
    cam->up = vec3_normalize(cam->up);
}

Camera *camera_create(void) {
    Camera *cam = malloc(sizeof(Camera));
    assert(cam);

    cam->pos = VEC3(0.0F);
    cam->world_up = VEC3(0.0F, 1.0F, 0.0F);

    cam->yaw = YAW;
    cam->pitch = PITCH;
    cam->last_xpos = 0.0F;
    cam->last_ypos = 0.0F;

    cam->ms_speed = SPEED;
    cam->sensitivity = SENSITIVITY;

    camera_update(cam);
    return cam;
}

void camera_destroy(Camera *cam) {
    free(cam);
}

void camera_stringify(Camera *cam) {
    LOG("[%.3f %.3f %.3f] pitch: %.3f yaw: %.3f",
        VEC3_GET(cam->pos, 0), VEC3_GET(cam->pos, 1), VEC3_GET(cam->pos, 2), cam->pitch, cam->yaw);
}

usize camera_size(void) {
    return sizeof(Camera);
}

void move(Camera *cam, Direction dir, f32 dt) {
    f32 velocity = cam->ms_speed * dt;

    fprintf(stdout, "%d\n", dir);

    switch (dir) {
        case FORWARD : cam->pos = vec3_add(cam->pos, vec3_muls(cam->front, velocity)); break;
        case BACKWARD: cam->pos = vec3_sub(cam->pos, vec3_muls(cam->front, velocity)); break;
        case RIGHT   : cam->pos = vec3_add(cam->pos, vec3_muls(cam->right, velocity)); break;
        case LEFT    : cam->pos = vec3_sub(cam->pos, vec3_muls(cam->right, velocity)); break;
        case UP      : cam->pos = vec3_add(cam->pos, vec3_muls(cam->up, velocity));    break;
        case DOWN    : cam->pos = vec3_sub(cam->pos, vec3_muls(cam->up, velocity));    break;
    }
}

void rotate(Camera *cam, f32 xpos, f32 ypos) {
    f32 x_offset = xpos - cam->last_xpos;
    f32 y_offset = cam->last_ypos - ypos;

    cam->last_xpos = xpos;
    cam->last_ypos = ypos;

    cam->yaw += x_offset * cam->sensitivity;
    cam->pitch += y_offset * cam->sensitivity;

    cam->pitch = MIN(MAX_PITCH, cam->pitch);
    cam->pitch = MAX(MIN_PITCH, cam->pitch);

    camera_update(cam);
}



