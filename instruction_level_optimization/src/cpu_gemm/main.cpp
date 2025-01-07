#include <iostream>
#include <mkl.h>
#include <mkl_spblas.h>

#include "cpu_gemm_runner.hpp"
#include "matrix.hpp"

using namespace std;

int main () {
    std::string file_path = "data/bcsstk16.mtx";
    //std::string file_path = "data/small.mtx";

    if (true) {
        CPUGEMMRunnerState state = create_runner_state(KernelType::SPGEMM__, KernelImplementation::MKL__, file_path);

        run_kernel(state);

        std::cout << "SPGEMM MKL Inspection time (ms): " << state.inspection_time_ms << endl;
        std::cout << "SPGEMM MKL Execution time (ms): " << state.execution_time_ms << endl;

        destroy_runner_state(state);
    }


    if (true) {
        CPUGEMMRunnerState state = create_runner_state(KernelType::SPMV__, KernelImplementation::MKL__, file_path);

        run_kernel(state);

        std::cout << "SPMV MKL Inspection time (ms): " << state.inspection_time_ms << endl;
        std::cout << "SPMV MKL Execution time (ms): " << state.execution_time_ms << endl;

        destroy_runner_state(state);
    }


    if (true) {
        CPUGEMMRunnerState state = create_runner_state(KernelType::SPMM__, KernelImplementation::MKL__, file_path);

        run_kernel(state);

        std::cout << "SPMM MKL Inspection time (ms): " << state.inspection_time_ms << endl;
        std::cout << "SPMM MKL Execution time (ms): " << state.execution_time_ms << endl;

        destroy_runner_state(state);
    }


    return 0;
}
