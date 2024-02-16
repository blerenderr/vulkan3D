#pragma once
#include "types.h"
#include "SDL/window.h"
#include "SDL/renderer.h"

#include <unistd.h>

typedef struct engine_t {
    b8 is_running;
} engine_t;

engine_t *engine;

b8 engine_start();
