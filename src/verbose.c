#include "verbose.h"

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
