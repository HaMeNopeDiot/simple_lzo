#ifndef VERBOSE_H
#define VERBOSE_H

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

extern bool Verbose;

int verbose(const char * format, ...);
void setVerbose(bool);

#endif /* VERBOSE_H */