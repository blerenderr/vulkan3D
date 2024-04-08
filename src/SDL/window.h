#pragma once
#include "../types.h"
#include <SDL_video.h>

#define SCREEN_HEIGHT 400
#define SCREEN_WIDTH 400

b8 window_init(u32 *extensionCount, const char **extensionNames);
SDL_Window* window_getMain();
u32 window_getMainID();
void window_destroy();

