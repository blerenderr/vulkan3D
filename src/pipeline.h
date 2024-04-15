#pragma once
#include "types.h"

typedef struct pipeline_t {
    void *vertCode;
    u32 vertCodeSize;
    void *fragCode;
    u32 fragCodeSize;
} pipeline_t;

void pipeline_init(char vertFilepath[], char fragFilepath[]);
void pipeline_cleanup();
