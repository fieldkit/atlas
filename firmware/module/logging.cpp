#include "logging.h"

#include <stdio.h>
#include <stdarg.h>

#include <Arduino.h>

void fk_logging_configure(const char *prefix) {
}

static void log(const char *format, va_list args) {
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    Serial.print(buffer);
}

void fkprintln(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(format, args);
    Serial.println();
    va_end(args);
}

void fkprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(format, args);
    va_end(args);
}
