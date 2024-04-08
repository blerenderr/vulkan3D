#include "renderer.h"
#include "../report.h"

SDL_Renderer *mainRenderer;

b8 renderer_init(SDL_Window *mainWindow) {
    SDL_Renderer *pRenderer = SDL_CreateRenderer(mainWindow, 0, SDL_RENDERER_ACCELERATED);
    if(pRenderer == NULL) {
        report_fatal("renderer_init()", "failed to create SDL renderer");
        return FALSE;
    }
    mainRenderer = pRenderer;
    return TRUE;
}

SDL_Renderer* renderer_getMain() {
    return mainRenderer;
}

b8 renderer_clear() {
    b8 result = 0;
    result += SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 0);
    result += SDL_RenderClear(mainRenderer);
    if(result != 0) {
        report_error("renderer_clear()", "SDL clearing functions failed");
    }
    return result;
}

void renderer_present() {
    SDL_RenderPresent(mainRenderer);
}

void renderer_destroy() {
    SDL_DestroyRenderer(mainRenderer);
}