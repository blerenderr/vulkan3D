#include "pipeline.h"
#include "utils.h"
#include <stdio.h>

pipeline_t *pipeline;

void pipeline_init(char vertFilepath[], char fragFilepath[]) {
    {
        pipeline_t temp;
        pipeline = &temp;
    }
    pipeline->vertCode = utils_readFile(vertFilepath, &pipeline->vertCodeSize);
    pipeline->fragCode = utils_readFile(fragFilepath, &pipeline->fragCodeSize);

    printf("vertex code size: %i bytes\nfragment code size: %i bytes\n", pipeline->vertCodeSize, pipeline->fragCodeSize);


}