#ifndef VERBOSE_H
#define VERBOSE_H

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

bool Verbose = false;


int verbose(const char * format, ...);
void setVerbose(bool);


void setVerbose(bool setting) {
    Verbose = setting;
}

int verbose(const char * format, ...) {
    if( !Verbose )
        return 0;

    va_list args;
    va_start(args, format);
    int ret = vprintf(format, args);
    va_end(args);

    return ret;
}

#endif /* VERBOSE_H */