//
// Created by Luis Ruisinger on 25.12.24.
//

#ifndef SOFTWARE_RATYTRACING_CAMERA_H
#define SOFTWARE_RATYTRACING_CAMERA_H

#include "defines.h"

typedef enum Direction_t {
    FORWARD, BACKWARD,
    LEFT, RIGHT,
    UP, DOWN
} Direction;

typedef struct Camera_t Camera;

usize camera_size(void);
Camera *camera_create(void);
void camera_destroy(Camera *cam);
void camera_stringify(Camera *cam);
void move(Camera *cam, Direction movement, f32 delta_time);
void rotate(Camera *cam, f32 xpos, f32 ypos);

#endif //SOFTWARE_RATYTRACING_CAMERA_H
