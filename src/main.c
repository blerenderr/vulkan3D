#include "main.h"

int main(int argc, char *argv[]) {
    printf("hello\n");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    engine_start();
    SDL_Quit();


    return 0;

}