#include "engine.h"
#include "SDL/window.h"
#include "SDL/renderer.h"
#include "SDL/input.h"
#include "pipeline.h"
#include "bigvulkan.h"

#include "report.h"

#include <unistd.h>

engine_t *engine;

b8 engine_start() {
    {   // create our engine and then store its address, malloc this when it gets big
        engine_t lEngine;
        lEngine.isRunning = TRUE;
        engine = &lEngine;
    }

    // variables for extensions required by SDL 
    u32 extensionCount = 30;
    char *exNames[30];
    const char **extensionNames = (const char **)exNames; // lol

    window_init(&extensionCount, extensionNames);
    renderer_init(window_getMain());

    SDL_Event event; // needs to be kept in scope or else input causes a segfault
    input_init(&event);

    pipeline_init("resource/simple_shader.vert.spv", "resource/simple_shader.frag.spv");

    bigvulkan_init(extensionCount, extensionNames);


    report_info("engine_start()", "inits complete, starting engine\n");
    while(engine->isRunning) {
        renderer_clear();
        // stuff

        if(!input_handle()) {
            engine->isRunning = FALSE;
        }

        SDL_SetRenderDrawColor(renderer_getMain(), 255,255,255,0);
        SDL_RenderDrawLine(renderer_getMain(), 0,0,400,400);

        renderer_present();
        usleep(33);
    }
    report_info("engine_start()", "engine has stopped, performing cleanup");
    bigvulkan_cleanup();
    renderer_destroy();
    window_destroy();
    return TRUE;
}

engine_t* engine_get() {
    return engine;
}