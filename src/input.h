#pragma once
#include "types.h"
#include <SDL_events.h>

typedef enum {
    FORWARD_BIT = 0x1,
    BACKWARD_BIT = 0x2,
    STRAFE_LEFT_BIT = 0x4,
    STRAFE_RIGHT_BIT = 0x8,
    JUMP_BIT = 0x10,
    CROUCH_BIT = 0x20,
    LOOK_UP_BIT = 0x40,
    LOOK_DOWN_BIT = 0x80,
    LOOK_LEFT_BIT = 0x100,
    LOOK_RIGHT_BIT = 0x200
} inputStateBits_t;


typedef struct {
    SDL_Event * event;
    u16 bits;
} input_t;

extern input_t * input_state;

void input_init();
b8 input_handle();
void input_cleanup();
