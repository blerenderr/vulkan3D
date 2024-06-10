#pragma once
#include "types.h"

void * utils_readFile(const char filename[], u32 *size, u32 bytesPerType);

void * utils_malloc(unsigned long size);
void utils_free(void *ptr);

b8 utils_writeFileString(const char filename[], const char message[]);

vec3_t newVec3(f32 x, f32 y, f32 z);

void utils_copy(void *dst, void *src, u32 size);