#pragma once
#include "../types.h"
#include <SDL_events.h>


typedef struct input_t {
    SDL_Event *event;
} input_t;

void input_init(SDL_Event *event);
b8 input_handle();
