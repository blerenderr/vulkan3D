#pragma once
#include "types.h"


void instance_init(u32 *extensionCount, const char **extensionNames);
void instance_destroy();

void instance_pickPhysicalDevice();
