#pragma once
#include "../types.h"
#include <SDL_render.h>

SDL_Renderer *mainRenderer;

b8 renderer_init(SDL_Window *mainWindow);
b8 renderer_clear();
void renderer_present();
void renderer_destroy();