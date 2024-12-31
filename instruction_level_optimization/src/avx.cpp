#include "avx.hpp"
#include "debug.hpp"

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
