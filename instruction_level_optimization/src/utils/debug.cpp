#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <string>

#include "debug.hpp"

namespace formats {
    const char *LIGHT_YELLOW = "\033[1;33m";
    const char *GREEN = "\033[0;32m";
    const char *RED = "\033[0;31m";
    const char *PURPLE = "\033[0;35m";
    const char *LIGHT_GRAY = "\033[0;37m";
    const char *ITALIC = "\033[3m";
    const char *BOLD = "\033[1m";
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

void _log_test_result(std::string test_id, bool verdict) {
    std::cout << formats::LIGHT_GRAY << formats::ITALIC << "------test------ " << formats::END;
    int num_dashes = std::max(30 - (int)test_id.size(), 0);
    std::cout << formats::LIGHT_YELLOW << test_id << formats::END << " ";
    for(int i = 0; i < num_dashes; ++i) std::cout << "-";
    std::cout << " ";
    if (verdict) {
        std::cout << formats::GREEN << "Pass" << formats::END << std::endl;
    }  
    else {
        std::cout << formats::RED << "Fail" << formats::END << std::endl;
    }
}