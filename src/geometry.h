#pragma once
#include "types.h"

#define VERTEX_BUFFER_SIZE 1024

typedef struct {
    vec3_t pos;
    vec3_t color;
} vertex_t;

extern vertex_t * geometry_vertices;
extern u32 geometry_verticesPos;

extern u16 * geometry_indices;
extern u32 geometry_indicesPos;

void geometry_init();
void geometry_addVertex(vertex_t vert);
void geometry_addIndex(u16 index);
void geometry_cleanup();
