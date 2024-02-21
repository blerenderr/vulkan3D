#include "renderer.h"

SDL_Renderer *mainRenderer;

b8 renderer_init(SDL_Window *mainWindow) {
    SDL_Renderer *pRenderer = SDL_CreateRenderer(mainWindow, 0, SDL_RENDERER_ACCELERATED);
    if(pRenderer == NULL) {
        printf("failed to create SDL renderer\n");
        return 1;
    }
    mainRenderer = pRenderer;
    return 0;
}

SDL_Renderer* renderer_getMain() {
    return mainRenderer;
}

b8 renderer_clear() {
    b8 result = 0;
    result += SDL_SetRenderDrawColor(mainRenderer, 64,64,64,0);
    result += SDL_RenderClear(mainRenderer);
    if(result > 0) {
        printf("renderer_clear failed\n");
    }
    return result;
}

void renderer_present() {
    SDL_RenderPresent(mainRenderer);
}

void renderer_destroy() {
    SDL_DestroyRenderer(mainRenderer);
}