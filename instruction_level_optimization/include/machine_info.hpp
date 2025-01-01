#pragma once 

#include <vector>
#include <cstdint>

#include "stdbool.h"

#ifdef __AVX2__
#pragma GCC target("avx2")
const bool AVX2_AVAILABLE = true;
#else 
const bool AVX2_AVAILABLE = false;
#endif

constexpr int SSE_B      = (1 << 0);
constexpr int SSE2_B     = (1 << 1);
constexpr int AVX_B      = (1 << 2);
constexpr int AVX2_B     = (1 << 3);
constexpr int AVX512F_B  = (1 << 4);
constexpr int AVX512VL_B = (1 << 5);

struct MachinInfo {
    int extension_mask;

    // the cache parameters for power cores
    uint32_t cache_line_size_b_p;
    uint32_t l1i_cache_size_kb_p;
    uint32_t l1d_cache_size_kb_p;
    uint32_t l2_cache_size_kb_p;
    uint32_t l3_cache_size_kb_p;

    // logical power cores
    uint32_t num_cpus_p;
    // physical power cores
    uint32_t num_cores_p; 
    // mapping from physical power cores to corresponding logical power cores
    std::vector< std::vector<uint32_t> > core_to_cpus_p;

    uint32_t freq_mhz_p;
};

extern MachinInfo INTEL_COREI5_13600K;

MachinInfo get_machine_info();

void print_extension_availability();
