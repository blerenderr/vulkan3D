#pragma once
#include "../types.h"
#include <SDL_render.h>


b8 renderer_init(SDL_Window *mainWindow);
SDL_Renderer* renderer_getMain();
b8 renderer_clear();
void renderer_present();
void renderer_destroy();