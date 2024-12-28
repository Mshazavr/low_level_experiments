#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <immintrin.h>
#include <cstdint>
#include <algorithm>
#include <string>

#include "utils/avx.hpp"
#include "utils/debug.hpp"
#include "utils/timer.hpp"
#include "utils/run_info.hpp"

enum SumMode{ 
    NAIVE, 
    UNROLL,
    SIMD,
    SIMD_UNROLL,
};

int32_t sum_naive(int32_t *a, uint32_t n) {
    int32_t sum = 0;
    for(uint32_t i = 0; i < n; ++i) {
        sum += a[i];
    }
    return sum;
}

int32_t sum_unroll(int32_t *a, uint32_t n) {
    // This is an example of instruction-level parallelism 
    // Unrolling takes advantage of the throuput of add inherently 
    // present due to pipelining, superscalar CPUs
    // optimal_num_workers = latency * throughput
    constexpr int STRIDE = 5;

    //int sums[STRIDE];
    /*#pragma GCC unroll STRIDE
    for (int j = 0; j < STRIDE; ++j) sums[j] = 0;
    */
    int32_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    int n_rounded = (int)n - (int)n % STRIDE;

    for(uint32_t i = 0; i < n_rounded; i += STRIDE) {
        /*#pragma GCC unroll STRIDE
        for (int j = 0; j < STRIDE; ++j) {
            sums[j] += a[i+j];
        }*/
        sum0 += a[i];
        sum1 += a[i+1];
        sum2 += a[i+2];
        sum3 += a[i+3];
        sum4 += a[i+4];
    }

    
    for (uint32_t i = n_rounded; i < n; ++i) {
        //sums[0] += a[i];
        sum0 += a[i];
    }
    

    /*int32_t sum = 0;
    #pragma GCC unroll STRIDE
    for (int j = 0; j < STRIDE; ++j) {
        sum += sums[j];
    }
    return sum;*/
    return sum0 + sum1 + sum2 + sum3 + sum4;
}


int32_t sum_simd(int32_t *a, uint32_t n) {
    // This function assumes &a is 32-byte aligned
    constexpr int STRIDE = 8;

    int n_rounded = (int)n - (int)n % STRIDE;

    __m256i sum_vec = _mm256_setzero_si256();
    for(uint32_t i = 0; i < n_rounded; i += STRIDE) {
        __m256i current_vec = _mm256_load_si256((__m256i*)(a+i));
        sum_vec = _mm256_add_epi32(sum_vec, current_vec);
    }

    
    alignas(32) int32_t sums[8];
    _mm256_storeu_si256((__m256i*)sums, sum_vec);

    int32_t sum = 0;
    for(int i = 0; i < 8; ++i) {
        sum += sums[i];
    }

    for (uint32_t i = n_rounded; i < n; ++i) {
        sum += a[i];
    }

    return sum;
}


int32_t sum_simd_unroll(int32_t *a, uint32_t n) {
    // This function assumes &a is 32-byte aligned
    constexpr int SIMD_STRIDE = 8;
    constexpr int UNROLL_STRIDE = 5;

    int n_rounded = (int)n - (int)n % (SIMD_STRIDE*UNROLL_STRIDE);

    __m256i sum_vec0 = _mm256_setzero_si256();
    __m256i sum_vec1 = _mm256_setzero_si256();
    __m256i sum_vec2 = _mm256_setzero_si256();
    __m256i sum_vec3 = _mm256_setzero_si256();
    __m256i sum_vec4 = _mm256_setzero_si256();
    for(uint32_t i = 0; i < n_rounded; i += SIMD_STRIDE*UNROLL_STRIDE) {
        __m256i current_vec0 = _mm256_load_si256((__m256i*)(a+i));
        __m256i current_vec1 = _mm256_load_si256((__m256i*)(a+i+SIMD_STRIDE));
        __m256i current_vec2 = _mm256_load_si256((__m256i*)(a+i+2*SIMD_STRIDE));
        __m256i current_vec3 = _mm256_load_si256((__m256i*)(a+i+3*SIMD_STRIDE));
        __m256i current_vec4 = _mm256_load_si256((__m256i*)(a+i+4*SIMD_STRIDE));
        sum_vec0 = _mm256_add_epi32(sum_vec0, current_vec0);
        sum_vec1 = _mm256_add_epi32(sum_vec1, current_vec1);
        sum_vec2 = _mm256_add_epi32(sum_vec2, current_vec2);
        sum_vec3 = _mm256_add_epi32(sum_vec3, current_vec3);
        sum_vec4 = _mm256_add_epi32(sum_vec4, current_vec4);
    }

    
    alignas(32) int32_t sums0[8];
    alignas(32) int32_t sums1[8];
    alignas(32) int32_t sums2[8];
    alignas(32) int32_t sums3[8];
    alignas(32) int32_t sums4[8];
    _mm256_storeu_si256((__m256i*)sums0, sum_vec0);
    _mm256_storeu_si256((__m256i*)sums1, sum_vec1);
    _mm256_storeu_si256((__m256i*)sums2, sum_vec2);
    _mm256_storeu_si256((__m256i*)sums3, sum_vec3);
    _mm256_storeu_si256((__m256i*)sums4, sum_vec4);

    int32_t sum = 0;
    for(int i = 0; i < 8; ++i) {
        sum += sums0[i]+sums1[i]+sums2[i]+sums3[i]+sums4[i];
    }

    for (uint32_t i = n_rounded; i < n; ++i) {
        sum += a[i];
    }

    return sum;
}


int run_sum(int32_t *a, uint32_t n, int32_t expected_output, SumMode mode) {
    _debug("running sum with mode %d...", mode);
    
    int start_ms = get_microseconds();
    int32_t result = 0;
         if (mode == SumMode::NAIVE)       result = sum_naive(a, n);
    else if (mode == SumMode::UNROLL)      result = sum_unroll(a, n);
    else if (mode == SumMode::SIMD)        result = sum_simd(a, n);
    else if (mode == SumMode::SIMD_UNROLL) result = sum_simd_unroll(a, n);
    else  _debug("mode %d not implemented", mode);
    int end_ms = get_microseconds();

    std::string verdict;
    if (expected_output == result) verdict = std::string(formats::GREEN) + "Pass" + std::string(formats::END);
    else                           verdict = std::string(formats::RED)   + "Fail" + std::string(formats::END);
    
    _debug("result: %d %s", result, verdict.c_str());
    _debug("elapsed time (s): %s%f%s", formats::LIGHT_YELLOW, (float)(end_ms - start_ms) / 1000000.0, formats::END);
    _debug_nl();

    return end_ms - start_ms;
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
         if ( mode == "naive" )       latency = run_sum(a, N, sum, SumMode::NAIVE);
    else if ( mode == "unroll" )      latency = run_sum(a, N, sum, SumMode::UNROLL);
    else if ( mode == "simd" )        latency = run_sum(a, N, sum, SumMode::SIMD);
    else if ( mode == "simd_unroll" ) latency = run_sum(a, N, sum, SumMode::SIMD_UNROLL);

    RunInfo result = {
        .name=mode, 
        .latency_ms=latency, 
        .attr_names={"n"}, 
        .attr_values{std::to_string(N)}
    };

    std::cout << result << std::endl;
    return 0;
}