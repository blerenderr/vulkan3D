#pragma once
#include "types.h"

typedef struct pipeline_t {
    u8 *vertCode;
    u32 vertCodeSize;
    u8 *fragCode;
    u32 fragCodeSize;
} pipeline_t;

void pipeline_init(char vertFilepath[], char fragFilepath[]);
