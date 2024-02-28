#pragma once
#include "types.h"

typedef struct engine_t {
    b8 is_running;
} engine_t;

b8 engine_start();
engine_t* engine_get();