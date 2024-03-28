#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

u8* utils_readFile(const char filename[], u32 *size) {
    FILE *file;
    file = fopen(filename, "r");

    if(file == NULL) {
        printf("couldn't get file pointer type\n");
        return 0;
    }

    // get size of file so we can malloc it
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    // pass back our filesize
    *size = fileSize;

    u8 *data = malloc(fileSize * sizeof(u8));

    fread(data, fileSize, 1, file);
    fclose(file);


    return data;
}