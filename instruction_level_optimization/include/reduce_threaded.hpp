#pragma once

#include <cstdint>
#include <pthread.h>

#include "sum_mode.hpp"

constexpr int NUM_THREADS = 10;

struct ThreadShared {
    // thread-specific 
    int32_t thread_idx;

    // shared reads
    int32_t *a; 
    uint32_t n;

    // shared writes
    int32_t *sum; 
    pthread_mutex_t *sum_mutex; 
};
typedef ThreadShared thread_arg_t;

// this function assumes the array address is 32-byte aligned
// if mode is THREADED_STRIDEAS_SIMD or THREADED_BLOCKS_SIMD
int32_t sum_threaded(int32_t *a, uint32_t n, SumMode mode);