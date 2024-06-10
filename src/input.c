#include "input.h"

input_t * input_state;
input_t ** state = &input_state;


void input_init() {
    *state = (input_t * )malloc(sizeof(input_t));
    (*state)->event = (SDL_Event * )malloc(sizeof(SDL_Event));
}

b8 input_handle() {
    if(SDL_PollEvent((*state)->event)) {
        u32 type = input_state->event->type;
        if(type == SDL_QUIT) { 
            return FALSE; 
        }
        switch((*state)->event->key.keysym.sym) {
            case 'q':
                return FALSE;
            break;
            case 'w':
                switch(type) {
                    case SDL_KEYDOWN:
                        input_state->bits |= FORWARD_BIT;
                    break;
                    case SDL_KEYUP:
                        input_state->bits ^= FORWARD_BIT;
                    break;
                }
            break;
            case 's':
                switch(type) {
                    case SDL_KEYDOWN:
                        input_state->bits |= BACKWARD_BIT;
                    break;
                    case SDL_KEYUP:
                        input_state->bits ^= BACKWARD_BIT;
                    break;
                }
            break;
            case 'a':
                switch(type) {
                    case SDL_KEYDOWN:
                        input_state->bits |= STRAFE_LEFT_BIT;
                    break;
                    case SDL_KEYUP:
                        input_state->bits ^= STRAFE_LEFT_BIT;
                    break;
                }
            break;
            case 'd':
                switch(type) {
                    case SDL_KEYDOWN:
                        input_state->bits |= STRAFE_RIGHT_BIT;
                    break;
                    case SDL_KEYUP:
                        input_state->bits ^= STRAFE_RIGHT_BIT;
                    break;
                }
            break;
            case 'o':
                switch(type) {
                    case SDL_KEYDOWN:
                        input_state->bits |= LOOK_UP_BIT;
                    break;
                    case SDL_KEYUP:
                        input_state->bits ^= LOOK_UP_BIT;
                    break;
                }
            break;
            case 'l':
                switch(type) {
                    case SDL_KEYDOWN:
                        input_state->bits |= LOOK_DOWN_BIT;
                    break;
                    case SDL_KEYUP:
                        input_state->bits ^= LOOK_DOWN_BIT;
                    break;
                }
            break;
            case 'k':
                switch(type) {
                    case SDL_KEYDOWN:
                        input_state->bits |= LOOK_LEFT_BIT;
                    break;
                    case SDL_KEYUP:
                        input_state->bits ^= LOOK_LEFT_BIT;
                    break;
                }
            break;
            case ';':
                switch(type) {
                    case SDL_KEYDOWN:
                        input_state->bits |= LOOK_RIGHT_BIT;
                    break;
                    case SDL_KEYUP:
                        input_state->bits ^= LOOK_RIGHT_BIT;
                    break;
                }
            break;

        }
    }
    return TRUE;
}

void input_cleanup() {
    free((*state)->event);
    free(*state);
}
