#include "main.h"
#include <SDL_video.h>

int main(int argc, char *argv[]) {
    printf("hello mate\n");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window *pWindow = SDL_CreateWindow("hi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, 0, SDL_RENDERER_SOFTWARE);

    while(1) {
        SDL_SetRenderDrawColor(pRenderer, 64,64,64,0);
        SDL_RenderClear(pRenderer);

        SDL_RenderPresent(pRenderer);
        sleep(1);
    }
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();


    return 0;

}