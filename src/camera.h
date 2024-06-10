#pragma once
#include "types.h"

typedef struct {
    f32 fov;
    vec3_t pos;
    vec3_t viewDir;
    vec3_t up;
    f32 nearClip;
    f32 farClip;
    f32 viewMatrix[16];
    f32 projMatrix[16];
} camera_t;

extern camera_t * mainCamera;

void camera_init(f32 fov, f32 nearClip, f32 farClip);
void camera_move();
void camera_updateMatrices();
void camera_cleanup();