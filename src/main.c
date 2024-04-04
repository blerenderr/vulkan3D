#include "main.h"
#include "report.h"

int main(int argc, char *argv[]) {
    report_clearLog();
    #ifdef DEBUG
    report_info("main()", "debug behavior is enabled");
    #endif
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    engine_start();
    SDL_Quit();


    return 0;

}