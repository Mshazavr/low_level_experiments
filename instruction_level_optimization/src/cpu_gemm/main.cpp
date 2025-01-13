#include <iostream>
#include <vector>
#include <mkl.h>
#include <mkl_spblas.h>

#include "cpu_gemm_runner.hpp"
#include "matrix.hpp"
#include "debug.hpp"

constexpr int32_t NUM_ITERATIONS = 20;
constexpr int32_t NUM_COLD_ITERATIONS = 5;

struct RunnerConfig {
    KernelType kernel_type;
    KernelImplementation kernel_implementation;
};
 
void _log_benchmark_prefix() {
    std::cout << formats::LIGHT_GRAY
              << formats::ITALIC
              << "------benchmarks------ "
              << formats::END;
}

void _log_running_kernel(const RunnerConfig &config) {
    _log_benchmark_prefix();
    std::cout << "Running kernels for: " 
              << formats::BOLD << KernelTypeNames[config.kernel_type] << ", " 
              << formats::BOLD << KernelImplementationNames[config.kernel_implementation] << formats::END
              << " ..." << std::endl;
}

void _log_running_matrix(const std::string &matrix_path) {
    _log_benchmark_prefix();
    std::string file_name = matrix_path.substr(matrix_path.find_last_of("/") + 1);
    std::cout << formats::PURPLE 
              << file_name 
              << formats::END 
              << " - Iterations Complete:" 
              << formats::GREEN
              << " 0 / " 
              << NUM_ITERATIONS
              << formats::END;
    std::cout.flush();
}

void _log_update_iteration(int32_t it) {
    // x / y
    std::cout << "\b\b\b\b\b";
    if constexpr (NUM_ITERATIONS > 9) {
        std::cout << '\b';
    }
    if (it > 10) {
        std::cout << '\b';
    }

    std::cout << formats::GREEN << it << " / " << NUM_ITERATIONS << formats::END;
    std::cout.flush();
}

void _log_elapsed_times(int32_t inspection_time_ms, int32_t execution_time_avg_warm_ms) {
    _log_benchmark_prefix();
    std::cout << "Inspection Time (ms): ------------------ " << formats::LIGHT_YELLOW << inspection_time_ms << formats::END << std::endl;

    _log_benchmark_prefix();
    std::cout << "Mean Execution Time (ms, warm cache): -- " << formats::LIGHT_YELLOW << execution_time_avg_warm_ms << formats::END << std::endl;
}

void run_benchmarks(
    std::vector< RunnerConfig > configs,
    std::vector< std::string > matrix_paths
) {
    for (const RunnerConfig &config: configs) {
        _log_running_kernel(config);

        for (const std::string &matrix_path: matrix_paths) {
            _log_running_matrix(matrix_path);

            int32_t inspection_time_ms;
            int32_t execution_time_total_ms;
            int32_t execution_time_avg_warm_ms;

            CPUGEMMRunnerState state = create_runner_state(config.kernel_type, config.kernel_implementation, matrix_path);
            run_inspection(state);
            inspection_time_ms = state.inspection_time_ms;
            
            execution_time_total_ms = 0;
            for (int32_t it = 0; it < NUM_ITERATIONS; ++it) {
                run_execution(state);
                if (it >= NUM_COLD_ITERATIONS) {
                    execution_time_total_ms += state.execution_time_ms;
                }

                _log_update_iteration(it+1);
            }
            std::cout << std::endl;

            execution_time_avg_warm_ms = execution_time_total_ms / (NUM_ITERATIONS - NUM_COLD_ITERATIONS);
            
            destroy_runner_state(state);

            _log_elapsed_times(inspection_time_ms, execution_time_avg_warm_ms);
        }
        std::cout << std::endl;
    }
}

int main () {
    std::vector< std::string > matrix_paths = {
        "../dlmc/transformer/random_pruning/0.5/body_encoder_layer_1_ffn_conv2_fully_connected.smtx"
        //,"data/af_3_k101.mtx"
        //,"data/bcsstk16.mtx"
        //,"data/small.mtx"
    };

    std::vector< RunnerConfig > configs = {
        { .kernel_type = KernelType::SPMV__, KernelImplementation::NAIVE__ },
        { .kernel_type = KernelType::SPMV__, KernelImplementation::MKL__ },
        //{ .kernel_type = KernelType::SPMV__, KernelImplementation::CUSTOM__ },
    };

    run_benchmarks(configs, matrix_paths);

    return 0;
}
