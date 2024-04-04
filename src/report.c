#include "report.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define REPORT_BUFFER_SIZE 256

char buffer1[REPORT_BUFFER_SIZE];
char buffer2[REPORT_BUFFER_SIZE];

void report_clearLog() {
    FILE *file;
    file = fopen("log.txt", "w");
    fclose(file);
}

void report_info(const char function[], const char message[], ...) {
    va_list args;
    va_start(args, message);

    vsnprintf(buffer1, REPORT_BUFFER_SIZE, message, args);
    snprintf(buffer2, REPORT_BUFFER_SIZE, "[INFO] %s: %s\n", function, buffer1);
    va_end(args);
    utils_writeFileString("log.txt", buffer2);
    printf("%s", buffer2);

}

void report_warning(const char function[], const char message[], ...) {
    va_list args;
    va_start(args, message);

    vsnprintf(buffer1, REPORT_BUFFER_SIZE, message, args);
    snprintf(buffer2, REPORT_BUFFER_SIZE, "[WARNING] %s: %s\n", function, buffer1);
    va_end(args);
    utils_writeFileString("log.txt", buffer2);
    printf("%s", buffer2);

}

void report_error(const char function[], const char message[], ...) {
    va_list args;
    va_start(args, message);

    vsnprintf(buffer1, REPORT_BUFFER_SIZE, message, args);
    snprintf(buffer2, REPORT_BUFFER_SIZE, "[ERROR] %s: %s\n", function, buffer1);
    va_end(args);
    utils_writeFileString("log.txt", buffer2);
    printf("%s", buffer2);

}

void report_fatal(const char function[], const char message[], ...) {
    va_list args;
    va_start(args, message);

    vsnprintf(buffer1, REPORT_BUFFER_SIZE, message, args);
    snprintf(buffer2, REPORT_BUFFER_SIZE, "[FATAL] %s: %s\n", function, buffer1);
    va_end(args);
    utils_writeFileString("log.txt", buffer2);
    printf("%s", buffer2);

    exit(1);
}
