#ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED

#include <stdint.h>

void fk_logging_configure(const char *prefix);
void fkprintf(const char *format, ...);
void fkprintln(const char *format, ...);

#endif
