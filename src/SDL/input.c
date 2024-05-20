#include "input.h"

input_t * input_state;
input_t ** state = &input_state;


void input_init() {
    *state = (input_t * )malloc(sizeof(input_t));
    (*state)->event = (SDL_Event * )malloc(sizeof(SDL_Event));
}

b8 input_handle() {
    if(SDL_PollEvent((*state)->event)) {
        if((*state)->event->type == SDL_QUIT) { 
            return FALSE; 
        }
        switch((*state)->event->key.keysym.sym) {
            case 'q':
                return FALSE;
            break;
        } 
    }
    return TRUE;
}

void input_cleanup() {
    free((*state)->event);
    free(*state);
}
