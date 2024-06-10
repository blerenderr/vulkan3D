#pragma once
#include "types.h"
#include <SDL_video.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

extern SDL_Window * mainWindow;

b8 window_init(u32 *extensionCount, const char **extensionNames);
void window_destroy();

