#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <immintrin.h>
#include <cstdint>
#include <algorithm>
#include <string>

#include "avx.hpp"
#include "debug.hpp"
#include "timer.hpp"
#include "run_info.hpp"
#include "reduce.hpp"
#include "reduce_threaded.hpp"


int run_sum(int32_t *a, uint32_t n, int32_t expected_output, SumMode mode) {
    _debug("running sum with mode %d...", mode);
    
    constexpr int NUM_EXPERIMENTS = 20;

    int total_ms = 0;
    for (int i = 0; i < NUM_EXPERIMENTS; ++i) {
        int start_ms = get_microseconds();
        int32_t result = 0;
             if ( mode == SumMode::NAIVE                 ) result = sum_naive(a, n);
        else if ( mode == SumMode::UNROLL                ) result = sum_unroll(a, n);
        else if ( mode == SumMode::SIMD                  ) result = sum_simd(a, n);
        else if ( mode == SumMode::SIMD_UNROLL           ) result = sum_simd_unroll(a, n);
        else if ( mode == SumMode::THREADED_STRIDES      ) result = sum_threaded(a, n, mode);
        else if ( mode == SumMode::THREADED_BLOCKS       ) result = sum_threaded(a, n, mode);
        else if ( mode == SumMode::THREADED_STRIDES_SIMD ) result = sum_threaded(a, n, mode);
        else if ( mode == SumMode::THREADED_BLOCKS_SIMD  ) result = sum_threaded(a, n, mode);
        else  _debug("mode %d not implemented", mode);
        int end_ms = get_microseconds();

        std::string verdict;
        if (expected_output == result) verdict = std::string(formats::GREEN) + "Pass" + std::string(formats::END);
        else                           verdict = std::string(formats::RED)   + "Fail" + std::string(formats::END);
        
        _debug("result: %d %s", result, verdict.c_str());
        _debug("elapsed time (s): %s%f%s", formats::LIGHT_YELLOW, (float)(end_ms - start_ms) / 1000000.0, formats::END);
        _debug_nl();

        total_ms += (end_ms - start_ms);
    }

    return total_ms / NUM_EXPERIMENTS;
}


int main (int argc, char **argv) {
    _debug("AVX2 availability: %d", AVX2_AVAILABLE);
    print_extension_availability();
    _debug_nl();

    const int N = 100000000;
    int32_t *a = (int32_t*)aligned_alloc(32, N * sizeof(int32_t));
    int32_t sum = 0;
    for (uint32_t i = 0; i < N; ++i) {
        a[i] = std::min((int)i, 50);
        sum += a[i];
    }

    std::string mode = argc < 2 ? "naive" : argv[1];
    int latency;
         if ( mode == "naive"                 ) latency = run_sum(a, N, sum, SumMode::NAIVE);
    else if ( mode == "unroll"                ) latency = run_sum(a, N, sum, SumMode::UNROLL);
    else if ( mode == "simd"                  ) latency = run_sum(a, N, sum, SumMode::SIMD);
    else if ( mode == "simd_unroll"           ) latency = run_sum(a, N, sum, SumMode::SIMD_UNROLL);
    else if ( mode == "threaded_strides"      ) latency = run_sum(a, N, sum, SumMode::THREADED_STRIDES);
    else if ( mode == "threaded_blocks"       ) latency = run_sum(a, N, sum, SumMode::THREADED_BLOCKS);
    else if ( mode == "threaded_strides_simd" ) latency = run_sum(a, N, sum, SumMode::THREADED_STRIDES_SIMD);
    else if ( mode == "threaded_blocks_simd"  ) latency = run_sum(a, N, sum, SumMode::THREADED_BLOCKS_SIMD);

    RunInfo result = {
        .name=mode, 
        .latency_ms=latency, 
        .attr_names={"n"}, 
        .attr_values{std::to_string(N)}
    };

    std::cout << result << std::endl;
    return 0;
}