#pragma once
#include "types.h"


void bigvulkan_init(u32 extensionCount, const char **extensionNames);
void bigvulkan_drawFrame();
void bigvulkan_cleanup();
