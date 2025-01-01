#include <iostream>
#include <cstdint>
#include <vector>
#include <pthread.h>

#include "single_threaded_shuffle.hpp"
#include "machine_info.hpp"
#include "run_info.hpp"
#include "timer.hpp"
#include "debug.hpp"

int time_shuffle(int32_t *a, uint32_t n) {
    constexpr int NUM_ITERATIONS = 10;
    constexpr int NUM_COLD_ITERATIONS = 5;

    int total_ms = 0;
    for (size_t i = 0; i < NUM_ITERATIONS; ++i) {
        int start_ms = get_microseconds();
        shuffle_array(a, n);
        int end_ms = get_microseconds();
        
        if (i >= NUM_COLD_ITERATIONS) {
            total_ms += (end_ms - start_ms);
        }
    }

    return total_ms / (NUM_ITERATIONS - NUM_COLD_ITERATIONS);

}

int main() {
    // Make sure the program is bound to a p-core
    {
        cpu_set_t cpu_set;
        pthread_t thread = pthread_self();
        CPU_ZERO(&cpu_set);
        CPU_SET(0, &cpu_set);
        pthread_setaffinity_np(thread, sizeof(cpu_set), &cpu_set);
    }

    MachinInfo machine_info = INTEL_COREI5_13600K;
    constexpr int MAX_N = 10000000;

    std::vector<RunInfo> runs;

    int32_t *a = (int32_t *)malloc(MAX_N * sizeof(int32_t));

    for (uint32_t i = 1; (1 << i) <= MAX_N; ++i) {
        _debug("Iteration %d", i);
        uint32_t n = (1 << i);
        int latency_ms = time_shuffle(a, n);
        runs.push_back({
            .name = "shuffle",
            .latency_ms = latency_ms,
            .attr_names = {
                "n", 
                "data_size_b", 
                "l1d_cache_size_b",
                "l2_cache_size_b",
                "l3_cache_size_b",
            },
            .attr_values = {
                std::to_string(n),
                std::to_string(n * sizeof(int32_t)),
                std::to_string(machine_info.l1d_cache_size_kb_p * 1000),
                std::to_string(machine_info.l2_cache_size_kb_p * 1000),
                std::to_string(machine_info.l3_cache_size_kb_p * 1000)
            }
        });
    }

    std::cout << runs << std::endl;
}