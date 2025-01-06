#include <iostream>
#include <mkl.h>
#include <mkl_spblas.h>

#include "cpu_gemm_runner.hpp"
#include "matrix.hpp"

using namespace std;

int main () {
    std::string file_path = "data/bcsstk16.mtx";

    CPUGEMMRunnerState state = create_runner_state(KernelType::SPGEMM__, KernelImplementation::MKL__, file_path);

    run_kernel(state);

    std::cout << "Inspection time (ms): " << state.inspection_time_ms << endl;
    std::cout << "Execution time (ms): " << state.execution_time_ms << endl;

    std::cout << state.matrix_C.num_values << std::endl;
    return 0;
}
