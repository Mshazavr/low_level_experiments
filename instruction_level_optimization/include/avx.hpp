#pragma once 

#include "stdbool.h"

#ifdef __AVX2__
#pragma GCC target("avx2")
const bool AVX2_AVAILABLE = true;
#else 
const bool AVX2_AVAILABLE = false;
#endif


void print_extension_availability();
