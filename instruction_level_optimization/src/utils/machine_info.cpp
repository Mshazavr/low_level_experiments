#include <cstdint>
#include <vector>

#include "machine_info.hpp"
#include "debug.hpp"

MachinInfo INTEL_COREI5_13600K = {
    .extension_mask = SSE_B | SSE2_B | AVX_B | AVX2_B,

    .cache_line_size_b_p = 64,
    .l1i_cache_size_kb_p = 32,
    .l1d_cache_size_kb_p = 48,
    .l2_cache_size_kb_p = 2048,
    .l3_cache_size_kb_p = 24000,

    .num_cpus_p = 12,
    .num_cores_p = 6,
    .core_to_cpus_p = {
        {0, 1},
        {2, 3},
        {4, 5},
        {6, 7},
        {8, 9},
        {10, 11},
    },
    
    .freq_mhz_p = 5100
};


MachinInfo get_machine_info() {
    // TODO
    return INTEL_COREI5_13600K;
}

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
