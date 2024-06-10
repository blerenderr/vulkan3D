#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* utils_readFile(const char filename[], u32 *size, u32 bytesPerType) {
    FILE *file;
    file = fopen(filename, "r");

    if(file == NULL) {
        printf("couldn't get file pointer type\n");
        return NULL;
    }

    // get size of file so we can malloc it
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    // pass back our filesize
    if(size != NULL) {
        *size = fileSize;
    }


    void *data = malloc(fileSize * bytesPerType);

    fread(data, fileSize, 1, file);
    fclose(file);


    return data;
}

void * utils_malloc(unsigned long size) {
    return malloc(size);
}
void utils_free(void *ptr) {
    free(ptr);
}

b8 utils_writeFileString(const char filename[], const char message[]) {
    FILE *file;
    file = fopen(filename, "a"); // append mode

    if(file == NULL) {
        return FALSE;
    }

    fprintf(file, "%s", message);

    fclose(file);

    return TRUE;

}

vec3_t newVec3(f32 x, f32 y, f32 z) {
    vec3_t result = {x,y,z};
    return result;
}

void utils_copy(void *dst, void *src, u32 size) {
    memcpy(dst, src, (unsigned long)size);
}