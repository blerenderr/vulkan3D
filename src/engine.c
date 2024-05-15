#include "engine.h"
#include "SDL/window.h"
#include "SDL/input.h"
#include "bigvulkan.h"

#include "report.h"

#include <unistd.h>

engine_t *engine;

b8 engine_start() {
    engine = malloc(sizeof(engine_t));
    engine->isRunning = TRUE;

    // variables for extensions required by SDL 
    u32 extensionCount = 30;
    char *exNames[30];
    const char **extensionNames = (const char **)exNames; // lol

    window_init(&extensionCount, extensionNames);

    SDL_Event event; // needs to be kept in scope or else input causes a segfault
    input_init(&event);

    bigvulkan_init(extensionCount, extensionNames);


    report_info("engine_start()", "inits complete, starting engine\n");
    while(engine->isRunning) {

        if(!input_handle()) {
            engine->isRunning = FALSE;
        }

        bigvulkan_drawFrame();

    }
    report_info("engine_start()", "engine has stopped, performing cleanup");
    bigvulkan_cleanup();
    window_destroy();
    free(engine);
    return TRUE;
}

engine_t* engine_get() {
    return engine;
}