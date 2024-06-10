#pragma once
#include "types.h"

typedef struct {
    f32 data[16];
} mat4_t;

void normalizeVec3(vec3_t *vec);
vec3_t crossVec3(vec3_t a, vec3_t b);
mat4_t newMat4();
mat4_t newIdentMat4();
f32 dotVec3(vec3_t a, vec3_t b);
void mulMat4(f32 *a, f32 *b);
void translateMat4(f32 *matrix, vec3_t pos);
vec3_t addVec3(vec3_t a, vec3_t b);

mat4_t lookAt(vec3_t pos, vec3_t target, vec3_t up);
mat4_t perspective(f32 fov, f32 aspect, f32 near, f32 far);