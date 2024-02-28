#include "engine.h"
#include "SDL/window.h"
#include "SDL/renderer.h"
#include "SDL/input.h"
#include <unistd.h>

engine_t *engine;

b8 engine_start() {
    {   // create our engine and then store its address
        engine_t lEngine;
        lEngine.is_running = TRUE;
        engine = &lEngine;
    }
    window_init();
    renderer_init(window_getMain());

    SDL_Event event; // needs to be kept in scope or else input causes a segfault
    input_init(&event);

    printf("starting engine\n");
    while(engine->is_running) {
        renderer_clear();
        // stuff

        if(!input_handle()) { engine->is_running = FALSE; }

        SDL_SetRenderDrawColor(renderer_getMain(), 0,0,0,0);
        SDL_RenderDrawLine(renderer_getMain(), 0,0,400,400);

        renderer_present();
        usleep(33);
    }
    renderer_destroy();
    window_destroy();
    return 0;
}

engine_t* engine_get() {
    return engine;
}