#include <string>
#include <cstdlib>

#include "custom_kernels.hpp"
#include "cpu_gemm_runner.hpp"
#include "timer.hpp"

/****************************************************************************/
/**** SPGEMM ****************************************************************/
/****************************************************************************/

void run_spgemm_custom_inspection(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();
    //...
    int32_t end_ms = get_microseconds();
    state.inspection_time_ms = end_ms - start_ms;;
}

void run_spgemm_custom_execution(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();



    int32_t end_ms = get_microseconds();
    state.execution_time_ms = end_ms - start_ms;
    return;
}

CPUGEMMRunnerState create_spgemm_custom_state(std::string matrix_file_path) {
    crs_matrix_f64 matrix_A = load_mtx_file_into_crs(matrix_file_path, NULL);
    crs_matrix_f64 matrix_B = load_mtx_file_into_crs(matrix_file_path, NULL);

    return {
        .type = KernelType::SPGEMM__,
        .implementation = KernelImplementation::CUSTOM__,
        .sparse_matrix_A = matrix_A,
        .sparse_matrix_B = matrix_B
    };
}



/****************************************************************************/
/**** SPMV ******************************************************************/
/****************************************************************************/

void run_spmv_custom_inspection(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();
    //...
    int32_t end_ms = get_microseconds();
    state.inspection_time_ms = end_ms - start_ms;;
}

void run_spmv_custom_execution(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();



    int32_t end_ms = get_microseconds();
    state.execution_time_ms = end_ms - start_ms;
    return;
}

CPUGEMMRunnerState create_spmv_custom_state(std::string matrix_file_path) {
    crs_matrix_f64 matrix_A = load_mtx_file_into_crs(matrix_file_path, NULL);
    
    double *vector_b = (double *)malloc(matrix_A.num_columns * sizeof(double));
    for(size_t i = 0; i < matrix_A.num_columns; ++i) {
        vector_b[i] = (double)(std::rand() / (RAND_MAX / 100));
    }

    return {
        .type = KernelType::SPMV__,
        .implementation = KernelImplementation::CUSTOM__,
        .sparse_matrix_A = matrix_A,
        .vector_b = vector_b
    };
}