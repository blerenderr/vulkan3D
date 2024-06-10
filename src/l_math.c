#include "l_math.h"
#include <math.h>
#include "utils.h"

void normalizeVec3(vec3_t *vec) {
    f32 resultant = sqrt(vec->x*vec->x + vec->y*vec->y + vec->z*vec->z);
    if(resultant == 0) {return;}
    resultant = 1.0f / resultant;
    vec->x *= resultant;
    vec->y *= resultant;
    vec->z *= resultant;
}

vec3_t crossVec3(vec3_t a, vec3_t b) {
    vec3_t result;
    result.x = a.y*b.z - a.z*b.y;
    result.y = a.z*b.x - a.x*b.z;
    result.z = a.x*b.y - a.y*b.x;
    return result;
}

mat4_t newMat4() {
    mat4_t result;
    for(u8 i = 0; i < 16; i++) {
        result.data[i] = 0.0f;
    }
    return result;
}

mat4_t newIdentMat4() {
    mat4_t result = newMat4();
    result.data[0] = result.data[5] = result.data[10] = result.data[15] = 1.0f;
    return result;
}

f32 dotVec3(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

void mulMat4(f32 *a, f32 *b) {
    f32 result[4][4] = {0};
    for(u8 i = 0; i < 4; i++) {
        for(u8 j = 0; j < 4; j++) {
            for(u8 k = 0; k < 4; k++) {
                result[i][j] += a[i*4 + k] * b[k*4 + j];
            }
        }
    }
    utils_copy(a, result, sizeof(f32) * 16);
}

void translateMat4(f32 *matrix, vec3_t pos) {
    mat4_t trans = newIdentMat4();
    trans.data[3] = pos.x;
    trans.data[7] = pos.y;
    trans.data[11] = pos.z;
    mulMat4(matrix, trans.data);
}

vec3_t addVec3(vec3_t a, vec3_t b) {
    vec3_t result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

mat4_t lookAt(vec3_t pos, vec3_t target, vec3_t up) {
    vec3_t forward = {target.x - pos.x,
                target.y - pos.y,
                target.z - pos.z};
    normalizeVec3(&forward);

    vec3_t side = crossVec3(forward, up);
    normalizeVec3(&side);

    up = crossVec3(side, forward);

    mat4_t result;
    result.data[0] = side.x;
    result.data[1] = up.x;
    result.data[2] = -forward.x;
    result.data[3] = 0.0f;
    result.data[4] = side.y;
    result.data[5] = up.y;
    result.data[6] = -forward.y;
    result.data[7] = 0.0f;
    result.data[8] = side.z;
    result.data[9] = up.z;
    result.data[10] = -forward.z;
    result.data[11] = 0.0f;
    result.data[12] = -dotVec3(side, pos);
    result.data[13] = -dotVec3(up, pos);
    result.data[14] = dotVec3(forward, pos);
    result.data[15] = 1.0f;

    return result;
}

mat4_t perspective(f32 fov, f32 aspect, f32 near, f32 far) {
    mat4_t result = newMat4();
    f32 halftan = tanf(fov/2.0f * DEG_TO_RAD);

    result.data[0] = 1.0f / (aspect * halftan);
    result.data[5] = 1.0f / halftan;
    result.data[10] = -((far + near) / (far - near));
    result.data[11] = -1.0f;
    result.data[14] = -((2 * far * near) / (far - near));
    return result;
}