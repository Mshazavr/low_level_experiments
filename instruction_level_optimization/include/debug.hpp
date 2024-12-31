#pragma once 

#include <cstdarg>
#include <cstdio>

namespace formats {
    extern const char *LIGHT_YELLOW;
    extern const char *GREEN;
    extern const char *RED;
    extern const char *PURPLE;
    extern const char *LIGHT_GRAY;
    extern const char *ITALIC;
    extern const char *END;
}

void _debug(const char *fmt, ...);
void _debug_nl();