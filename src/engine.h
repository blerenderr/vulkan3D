#pragma once
#include "types.h"

typedef struct engine_t {
    b8 isRunning;
} engine_t;

b8 engine_start();
engine_t* engine_get();