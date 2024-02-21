#pragma once
#include "../types.h"
#include <SDL_video.h>

#define SCREEN_HEIGHT 400
#define SCREEN_WIDTH 400

b8 window_init();
SDL_Window* window_getMain();
void window_destroy();

