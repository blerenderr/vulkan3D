#include "engine.h"
#include "SDL/renderer.h"
#include <SDL_render.h>

b8 engine_start() {
    {
        engine_t lEngine;
        lEngine.is_running = TRUE;
        engine = &lEngine;
    }
    window_init();
    renderer_init(mainWindow);
    printf("starting engine\n");
    while(engine->is_running) {
        renderer_clear();
        // stuff
        SDL_SetRenderDrawColor(mainRenderer, 0,0,0,0);
        SDL_RenderDrawLine(mainRenderer, 0,0,400,400);

        renderer_present();
        usleep(33);
    }
    renderer_destroy();
    window_destroy();
    return 0;
}
