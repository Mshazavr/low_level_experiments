#include <cstdarg>
#include <cstdio>

#include "debug.hpp"

namespace formats {
    const char *LIGHT_YELLOW = "\033[1;33m";
    const char *GREEN = "\033[0;32m";
    const char *RED = "\033[0;31m";
    const char *PURPLE = "\033[0;35m";
    const char *LIGHT_GRAY = "\033[0;37m";
    const char *ITALIC = "\033[3m";
    const char *END = "\033[0m";
}

void _debug(const char *fmt, ...) {
#ifdef DEBUG

    va_list args;
    va_start(args, fmt);

    std::fprintf(stderr, "%s", formats::LIGHT_GRAY);
    std::fprintf(stderr, "%s", formats::ITALIC);
    std::fprintf(stderr, "------debug------ ");
    std::fprintf(stderr, "%s", formats::END);

    std::fprintf(stderr, "%s", formats::PURPLE);
    std::vfprintf(stderr, fmt, args);

    std::fprintf(stderr, "%s", formats::END);
    std::fprintf(stderr, "\n");

    va_end(args);

#endif
}

void _debug_nl() {
#ifdef DEBUG
    _debug("%s", "");
#endif
}