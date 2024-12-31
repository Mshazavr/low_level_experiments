#pragma once

#include <cstdint>

int32_t sum_naive(int32_t *a, uint32_t n);

int32_t sum_unroll(int32_t *a, uint32_t n);

// This function assumes &a is 32-byte aligned
int32_t sum_simd(int32_t *a, uint32_t n);

// This function assumes &a is 32-byte aligned
int32_t sum_simd_unroll(int32_t *a, uint32_t n);
