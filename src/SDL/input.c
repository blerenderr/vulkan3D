#include "input.h"

input_t input;

void input_init(SDL_Event *event) {
    input.event = event;
}

b8 input_handle() {
    if(SDL_PollEvent(input.event)) {
        if(input.event->type == SDL_QUIT) { 
            return FALSE; 
        }
        switch(input.event->key.keysym.sym) {
            case 'q':
                return FALSE;
            break;
        } 
    }
    return TRUE;
}