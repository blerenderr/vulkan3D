#pragma once
#include "../types.h"
#include <SDL_video.h>

#define SCREEN_HEIGHT 400
#define SCREEN_WIDTH 400


SDL_Window *mainWindow;
b8 window_init();
void window_destroy();
