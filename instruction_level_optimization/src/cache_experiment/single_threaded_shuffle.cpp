#include <cstdlib>

#include "machine_info.hpp"
#include "debug.hpp"
#include "single_threaded_shuffle.hpp"

void shuffle_array(int32_t *a, uint32_t n) {
    constexpr int NUM_ITERATIONS = 10000000;
    for (size_t i = 0; i < NUM_ITERATIONS; ++i) {
        uint32_t rand_ind1 = rand() % n;
        uint32_t rand_ind2 = rand() % n;
        std::swap(a[rand_ind1], a[rand_ind2]);
    }
}