#include <string>
#include <cstring>

#include "naive_kernels.hpp"
#include "cpu_gemm_runner.hpp"
#include "timer.hpp"


/****************************************************************************/
/**** SPGEMM ****************************************************************/
/****************************************************************************/

void run_spgemm_naive_inspection(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();
    // ...
    int32_t end_ms = get_microseconds();
    state.inspection_time_ms = end_ms - start_ms;
    return;
}

void run_spgemm_naive_execution(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();
    // ...
    int32_t end_ms = get_microseconds();
    state.execution_time_ms = end_ms - start_ms;
    return;
}

CPUGEMMRunnerState create_spgemm_naive_state(std::string matrix_file_path) {
    return {};
}


/****************************************************************************/
/**** SPMV ******************************************************************/
/****************************************************************************/

void run_spmv_naive_inspection(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();
    //...
    int32_t end_ms = get_microseconds();
    state.inspection_time_ms = end_ms - start_ms;;
}

void run_spmv_naive_execution(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();

    state.vector_c = (double *)malloc(state.sparse_matrix_A.num_rows * sizeof(double));
    memset(state.vector_c, 0, state.sparse_ccs_matrix_A.num_rows * sizeof(double));

    for(int32_t row = 0; row < state.sparse_matrix_A.num_rows; ++row) {
        for(
            int32_t ind = state.sparse_matrix_A.row_start[row]; 
            ind < state.sparse_matrix_A.row_start[row+1]; 
            ++ind
        ) {
            state.vector_c[row] += (
                state.sparse_matrix_A.values[ind] * 
                state.vector_b[state.sparse_matrix_A.columns[ind]]
            ); 
        }
    }

    int32_t end_ms = get_microseconds();
    state.execution_time_ms = end_ms - start_ms;
    return;
}

CPUGEMMRunnerState create_spmv_naive_state(std::string matrix_file_path) {
    crs_matrix_f64 matrix_A = load_file_into_crs(matrix_file_path, NULL);

    double *vector_b = (double *)malloc(matrix_A.num_columns * sizeof(double));
    for(size_t i = 0; i < matrix_A.num_columns; ++i) {
        vector_b[i] = (double)(std::rand() / (RAND_MAX / 100));
    }

    return {
        .type = KernelType::SPMV__,
        .implementation = KernelImplementation::NAIVE__,
        .sparse_matrix_A = matrix_A,
        .vector_b = vector_b,
    };
}



/****************************************************************************/
/**** SPMM ******************************************************************/
/****************************************************************************/

void run_spmm_naive_inspection(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();
    // ...
    int32_t end_ms = get_microseconds();
    state.inspection_time_ms = end_ms - start_ms;;
}

void run_spmm_naive_execution(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();
    // ...
    int32_t end_ms = get_microseconds();
    state.execution_time_ms = end_ms - start_ms;
    return;
}

CPUGEMMRunnerState create_spmm_naive_state(std::string matrix_file_path) {
    return {};
}