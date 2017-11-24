#include "logging.h"

#include <Arduino.h>

#include <stdio.h>
#include <stdarg.h>

uint8_t fk_logging_configure(const char *prefix) {
    return 0;
}

uint8_t fkprintf(const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    Serial.print(buffer);
    va_end(args);

    return 0;
}
