#include "camera.h"
#include "types.h"
#include "utils.h"
#include "l_math.h"
#include "input.h"
#include "window.h"

camera_t * mainCamera;

void camera_init(f32 fov, f32 nearClip, f32 farClip) {
    mainCamera = utils_malloc(sizeof(camera_t));
    mainCamera->fov = fov;
    mainCamera->pos = newVec3(0.0f, 0.0f, 10.0f);
    mainCamera->viewDir = newVec3(0.0f, 0.0f, -1.0f);
    mainCamera->up = newVec3(0.0f, 1.0f, 0.0f);
    mainCamera->nearClip = nearClip;
    mainCamera->farClip = farClip;
}

void camera_move() {
    u16 bits = input_state->bits;
    if(bits & FORWARD_BIT) {
        mainCamera->pos.z--;
    }
    if(bits & BACKWARD_BIT) {
        mainCamera->pos.z++;
    }
    if(bits & STRAFE_LEFT_BIT) {
        mainCamera->pos.x--;
    }
    if(bits & STRAFE_RIGHT_BIT) {
        mainCamera->pos.x++;
    }
    if(bits & LOOK_UP_BIT) {
        mainCamera->viewDir.y += 0.1f;
    }
    if(bits & LOOK_DOWN_BIT) {
        mainCamera->viewDir.y -= 0.1f;
    }
    if(bits & LOOK_LEFT_BIT) {
        mainCamera->viewDir.x -= 0.1f;
    }
    if(bits & LOOK_RIGHT_BIT) {
        mainCamera->viewDir.x += 0.1f;
    }
}

void updateViewMatrix() {
    mat4_t view = lookAt(mainCamera->pos, addVec3(mainCamera->pos, mainCamera->viewDir), mainCamera->up);
    utils_copy( mainCamera->viewMatrix, view.data, sizeof(f32) * 16);
}


void updateProjMatrix() {
    mat4_t proj = perspective(mainCamera->fov, SCREEN_WIDTH / (f32) SCREEN_HEIGHT, mainCamera->nearClip, mainCamera->farClip);
    utils_copy(mainCamera->projMatrix, proj.data, sizeof(f32) * 16);
}

void camera_updateMatrices() {
    updateViewMatrix();
    updateProjMatrix();
}

void camera_cleanup() {
    free(mainCamera);
}