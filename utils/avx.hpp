#pragma once 

#include "stdbool.h"
#include <iostream>

#include "utils/debug.hpp"

#ifdef __AVX2__
#pragma GCC target("avx2")
const bool AVX2_AVAILABLE = true;
#else 
const bool AVX2_AVAILABLE = false;
#endif





void print_extension_availability() {
    _debug("%s", "+-----------+-----------+");
    _debug("%s", "| Extension | Supported |");
    _debug("%s", "+-----------+-----------+");
    _debug(      "| sse       | %s|", __builtin_cpu_supports("sse")      ? "yes       " : "no        ");
    _debug("%s", "+-----------+-----------+");
    _debug(      "| sse2      | %s|", __builtin_cpu_supports("sse2")     ? "yes       " : "no        ");
    _debug("%s", "+-----------+-----------+");
    _debug(      "| avx       | %s|", __builtin_cpu_supports("avx")      ? "yes       " : "no        ");
    _debug("%s", "+-----------+-----------+");
    _debug(      "| avx2      | %s|", __builtin_cpu_supports("avx2")     ? "yes       " : "no        ");
    _debug("%s", "+-----------+-----------+");
    _debug(      "| avx512f   | %s|", __builtin_cpu_supports("avx512f")  ? "yes       " : "no        ");
    _debug("%s", "+-----------+-----------+");
    _debug(      "| avx512vl  | %s|", __builtin_cpu_supports("avx512vl") ? "yes       " : "no        ");
    _debug("%s", "+-----------+-----------+");
}
