#include "engine.h"
#include "SDL/window.h"
#include "SDL/input.h"
#include "bigvulkan.h"

#include "geometry.h"
#include "report.h"

#include <unistd.h>
#include <time.h>

engine_t * engine_state;


b8 engine_start() {
    engine_state = malloc(sizeof(engine_t));
    engine_state->isRunning = TRUE;

    // variables for extensions required by SDL 
    u32 extensionCount = 30;
    char *exNames[30];
    const char **extensionNames = (const char **)exNames; // lol

    window_init(&extensionCount, extensionNames);

    input_init();

    geometry_init();
    {
        vertex_t a = {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}};
        vertex_t b = {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}};
        vertex_t c = {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}};
        vertex_t d = {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}};
        vertex_t verts[] = {a, b, c, d};
        u16 indices[] = {0, 1, 2, 2, 3, 0};
        for(u16 i = 0; i < 4; i++) {
            geometry_addVertex(verts[i]);
        }
        for(u16 i = 0; i < 6; i++) {
            geometry_addIndex(indices[i]);
        }
    }

    bigvulkan_init(extensionCount, extensionNames);


    report_info("engine_start()", "inits complete, starting engine\n");
    u32 usec = 0;
    while(engine_state->isRunning) {

        if(!input_handle()) {
            engine_state->isRunning = FALSE;
        }

        // clock_t start = clock();
        bigvulkan_drawFrame();
        // clock_t diff = clock() - start;
        // usec = 1000000 * diff / CLOCKS_PER_SEC;

    }
    report_info("engine_start()", "engine has stopped, performing cleanup");
    bigvulkan_cleanup();
    geometry_cleanup();
    input_cleanup();
    window_destroy();
    free(engine_state);
    return TRUE;
}
