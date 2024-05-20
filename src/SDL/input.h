#pragma once
#include "../types.h"
#include <SDL_events.h>


typedef struct {
    SDL_Event * event;
} input_t;

extern input_t * input_state;

void input_init();
b8 input_handle();
void input_cleanup();
