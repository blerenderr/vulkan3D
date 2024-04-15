#pragma once
#include "types.h"

void* utils_readFile(const char filename[], u32 *size, u32 bytesPerType);
void utils_free(void *ptr);

b8 utils_writeFileString(const char filename[], const char message[]);
