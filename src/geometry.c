#include "geometry.h"
#include "report.h"
#include <stdlib.h>

vertex_t * geometry_vertices;
u32 geometry_verticesPos;
// cheeky aliasing
vertex_t ** vertices = &geometry_vertices;
u32 * verticesPos = &geometry_verticesPos;


u16 * geometry_indices;
u32 geometry_indicesPos;

u16 ** indices = &geometry_indices;
u32 * indicesPos = &geometry_indicesPos;


void geometry_init() {
    size_t size = sizeof(vertex_t) * VERTEX_BUFFER_SIZE;
    *vertices = malloc(size);
    report_info("geometry_init()", "size of vertex_t buffer: %d", size);
    *verticesPos = 0;

    *indices = malloc(sizeof(u16) * VERTEX_BUFFER_SIZE);
    *indicesPos = 0;
}

void geometry_addVertex(vertex_t vert) {
    (*vertices)[*verticesPos] = vert;
    *verticesPos = (*verticesPos + 1) % 1024;
}

void geometry_addIndex(u16 index) {
    (*indices)[*indicesPos] = index;
    *indicesPos = (*indicesPos + 1) % 1024;
}

void geometry_cleanup() {
    free(*vertices);
    free(*indices);
}