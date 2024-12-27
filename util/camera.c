//
// Created by Luis Ruisinger on 25.12.24.
//

#include <assert.h>

#include "camera.h"
#include "vec3.h"

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

    //fprintf(stdout, "%.2f, %.2f, %.2f\n", GET_VEC_X(cam->world_up), GET_VEC_Y(cam->world_up), GET_VEC_Z(cam->world_up));

    cam->front = VEC3(
            cosf_yaw * cosf_pitch,
            sinf(TO_RADIANS(cam->pitch)),
            sinf(TO_RADIANS(cam->yaw)) * cosf_pitch);
    cam->front = normalize(cam->front);

    //fprintf(stdout, "%.2f, %.2f, %.2f\n", GET_VEC_X(cam->front), GET_VEC_Y(cam->front), GET_VEC_Z(cam->front));

    cam->right = cross(cam->front, cam->world_up);
    cam->right = normalize(cam->right);

    //fprintf(stdout, "%.2f, %.2f, %.2f\n", GET_VEC_X(cam->right), GET_VEC_Y(cam->right), GET_VEC_Z(cam->right));

    cam->up = cross(cam->right, cam->front);
    cam->up = normalize(cam->up);

    //fprintf(stdout, "%.2f, %.2f, %.2f\n", GET_VEC_X(cam->up), GET_VEC_Y(cam->up), GET_VEC_Z(cam->up));
    //exit(-1);
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
    fprintf(stdout,
            "[%.3f %.3f %.3f] pitch: %.3f yaw: %.3f\n",
            GET_VEC_X(cam->pos), GET_VEC_Y(cam->pos), GET_VEC_Z(cam->pos), cam->pitch, cam->yaw);
}

usize camera_size(void) {
    return sizeof(Camera);
}

void move(Camera *cam, Direction dir, f32 dt) {
    f32 velocity = cam->ms_speed * dt;

    fprintf(stdout, "%d\n", dir);

    switch (dir) {
        case FORWARD : cam->pos = add_vec(cam->pos, mul_scalar(cam->front, velocity)); break;
        case BACKWARD: cam->pos = sub_vec(cam->pos, mul_scalar(cam->front, velocity)); break;
        case RIGHT   : cam->pos = add_vec(cam->pos, mul_scalar(cam->right, velocity)); break;
        case LEFT    : cam->pos = sub_vec(cam->pos, mul_scalar(cam->right, velocity)); break;
        case UP      : cam->pos = add_vec(cam->pos, mul_scalar(cam->up, velocity));    break;
        case DOWN    : cam->pos = sub_vec(cam->pos, mul_scalar(cam->up, velocity));    break;
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



