#include "window.h"

b8 window_init() {
    SDL_Window *pWindow = SDL_CreateWindow("hi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(pWindow == NULL) {
        printf("failed to create window\n");
        return 1;
    }
    mainWindow = pWindow;
    return 0;
}

void window_destroy() {
    SDL_DestroyWindow(mainWindow);
}