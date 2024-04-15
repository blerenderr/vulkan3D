#include "pipeline.h"
#include "utils.h"
#include "report.h"

pipeline_t *pipeline;

void pipeline_init(char vertFilepath[], char fragFilepath[]) {
    {
        pipeline_t temp;
        pipeline = &temp;
    }
    pipeline->vertCode = utils_readFile(vertFilepath, &pipeline->vertCodeSize, 1);
    pipeline->fragCode = utils_readFile(fragFilepath, &pipeline->fragCodeSize, 1);

    report_info("pipeline_init()","vertex code size: %u bytes\tfragment code size: %u bytes", pipeline->vertCodeSize, pipeline->fragCodeSize);


}
void pipeline_cleanup() {
    utils_free(pipeline->vertCode);
    utils_free(pipeline->fragCode);

}