#include "main.h"

int main(int argc, char *argv[]) {
    #ifdef DEBUG
    printf("debug behavior is enabled\n");
    #endif
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    engine_start();
    SDL_Quit();


    return 0;

}