#pragma once
#include "types.h"

// TODO: add timestamps

void report_clearLog();

// useful info on what the program is doing
void report_info(const char function[], const char message[], ...);

// not intended and may be a problem
void report_warning(const char function[], const char message[], ...);

// there is a problem but we can keep going
void report_error(const char function[], const char message[], ...);

// big problem, program can't continue
void report_fatal(const char function[], const char message[], ...);


