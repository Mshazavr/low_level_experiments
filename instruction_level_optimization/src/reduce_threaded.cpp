#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <immintrin.h>
#include <cstdint>
#include <algorithm>
#include <string>
#include <pthread.h>

#include "avx.hpp"
#include "debug.hpp"
#include "timer.hpp"
#include "run_info.hpp"
#include "reduce.hpp"
#include "reduce_threaded.hpp"


void *sum_thread_strides_kernel(void *arg) {
    thread_arg_t *thread_arg = (thread_arg_t *)arg;
    int32_t sum = 0;
    for(int32_t i = thread_arg->thread_idx; i < thread_arg->n; i += NUM_THREADS) {
        sum += thread_arg->a[i];
    }

    // accessing shared sum variable
    // need to lock first
    pthread_mutex_lock(thread_arg->sum_mutex); 
    *thread_arg->sum += sum; 
    pthread_mutex_unlock(thread_arg->sum_mutex);

    pthread_exit(arg);
}

void *sum_thread_blocks_kernel(void *arg) {
    thread_arg_t *thread_arg = (thread_arg_t *)arg;
    int32_t sum = 0;

    // compute the segment endpoints
    uint32_t block_size = (thread_arg->n / NUM_THREADS) + 1;
    uint32_t start = thread_arg->thread_idx * block_size;
    uint32_t end = std::min((thread_arg->thread_idx + 1) * block_size, thread_arg->n);

    for(int32_t i = start; i < end; i++) {
        sum += thread_arg->a[i];
    }

    // accessing shared sum variable
    // need to lock first
    pthread_mutex_lock(thread_arg->sum_mutex); 
    *thread_arg->sum += sum; 
    pthread_mutex_unlock(thread_arg->sum_mutex);

    pthread_exit(arg);
}

void *sum_thread_strides_simd_kernel(void *arg) {
    constexpr int SIMD_STRIDE = 8;
    thread_arg_t *thread_arg = (thread_arg_t *)arg;
    int32_t sum = 0;

    // the number of elements should be a multiple of SIMD_STRIDE 
    // we will add the remaining elements with regular instructions 
    uint32_t n_rounded = thread_arg->n - (thread_arg->n)%SIMD_STRIDE;

    __m256i sum_vec = _mm256_setzero_si256();
    alignas(32) int32_t simd_buffer[8];
    for(int32_t i = thread_arg->thread_idx; i < n_rounded; i += NUM_THREADS * SIMD_STRIDE) {
        for (int32_t j = 0; j < SIMD_STRIDE; ++j) {
            simd_buffer[j] = thread_arg->a[i + j * NUM_THREADS];
        }
        __m256i current_vec = _mm256_load_si256((__m256i*)simd_buffer);
        sum_vec = _mm256_add_epi32(sum_vec, current_vec);
    }

    _mm256_storeu_si256((__m256i*)simd_buffer, sum_vec);
    for(int i = 0; i < 8; ++i) {
        sum += simd_buffer[i];
    }

    // add the tail
    for (uint32_t i = n_rounded; i < thread_arg->n; ++i) {
        sum += thread_arg->a[i];
    }

    // accessing shared sum variable
    // need to lock first
    pthread_mutex_lock(thread_arg->sum_mutex); 
    *thread_arg->sum += sum; 
    pthread_mutex_unlock(thread_arg->sum_mutex);

    pthread_exit(arg);
}

// This function assumes the array address is 32-byte aligned
void *sum_thread_blocks_simd_kernel(void *arg) {
    thread_arg_t *thread_arg = (thread_arg_t *)arg;
    int32_t sum = 0;

    // compute the segment endpoints
    uint32_t block_size = (thread_arg->n / NUM_THREADS) + 1;
    uint32_t start = thread_arg->thread_idx * block_size;
    int32_t end = std::min((thread_arg->thread_idx + 1) * block_size, thread_arg->n);

    // we meed trim off the edges to work with endpoints that are 
    // multiples of 8 to be able to safely call SIMD instructions
    int32_t safe_start = (start | 7) + 1;
    int32_t safe_end = end - end%8;
    uint32_t safe_size = (safe_end > safe_start) ? safe_end - safe_start : 0;

    sum += sum_simd_unroll(thread_arg->a + safe_start, safe_size);
    for(int32_t i = start; i < safe_start; i++) {
        sum += thread_arg->a[i];
    }
    for(int32_t i = safe_end; i < end; i++) {
        sum += thread_arg->a[i];
    }

    // accessing shared sum variable
    // need to lock first
    pthread_mutex_lock(thread_arg->sum_mutex); 
    *thread_arg->sum += sum; 
    pthread_mutex_unlock(thread_arg->sum_mutex);

    pthread_exit(arg);
}

// This function assumes the array address is 32-byte aligned
// if mode is THREADED_STRIDEAS_SIMD or THREADED_BLOCKS_SIMD
int32_t sum_threaded(int32_t *a, uint32_t n, SumMode mode) {
    int32_t sum = 0; 

    // Create Mutex
    pthread_mutex_t sum_mutex; 
    pthread_mutex_init(&sum_mutex, NULL);

    // Create thread attr object
    pthread_attr_t thread_attr; 
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

    // Starts the threads
    pthread_t threads[NUM_THREADS];
    thread_arg_t thread_args[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_args[i] = {
            .thread_idx = i, 
            .a = a,
            .n = n, 
            .sum = &sum,
            .sum_mutex = &sum_mutex
        }; 

        void *(*thread_function)(void *);
        if      ( mode == SumMode::THREADED_STRIDES      ) thread_function = sum_thread_strides_kernel;
        else if ( mode == SumMode::THREADED_BLOCKS       ) thread_function = sum_thread_blocks_kernel;
        else if ( mode == SumMode::THREADED_STRIDES_SIMD ) thread_function = sum_thread_strides_simd_kernel;
        else if ( mode == SumMode::THREADED_BLOCKS_SIMD  ) thread_function = sum_thread_blocks_simd_kernel;
        else {
            _debug("invalid sum mode for threaded sum: %d. Exiting...", mode);
            exit(-1);
        }

        int rc = pthread_create(&threads[i], &thread_attr, thread_function, (void *)&thread_args[i]);

        if (rc != 0) {
            _debug("failed to create thread %d. Exiting...", i);
            exit(-1);
        }
    }

    // Block on the threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        int rc = pthread_join(threads[i], NULL);
        if (rc != 0) {
            _debug("thread %d failed. Exiting...", i);
            exit(-1);
        }
    }

    // Destroy Mutex and thread attr object
    pthread_mutex_destroy(&sum_mutex);
    pthread_attr_destroy(&thread_attr);

    return sum;
}
