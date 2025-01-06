#include <string>
#include <mkl.h>
#include <mkl_spblas.h>

#include "mkl_kernels.hpp"
#include "cpu_gemm_runner.hpp"
#include "timer.hpp"

void run_spgemm_mkl_inspection(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();
    // TODO: best faith available optimization for mkl
    int32_t end_ms = get_microseconds();
    state.inspection_time_ms = end_ms - start_ms;
    return;
}

void run_spgemm_mkl_execution(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();

    sparse_matrix_t mkl_matrix_C;

    mkl_sparse_spmm(
        SPARSE_OPERATION_NON_TRANSPOSE, 
        state.mkl_matrix_A, 
        state.mkl_matrix_B, 
        &mkl_matrix_C
    );

    int32_t end_ms = get_microseconds();
    state.execution_time_ms = end_ms - start_ms;

    // extraction time shouldn't count towards mkl execution time

    crs_matrix_f64 matrix_C = {};
    sparse_index_base_t indexing;
    int32_t *row_end = matrix_C.row_start + 1;

    mkl_sparse_d_export_csr(
        mkl_matrix_C, &indexing, 
        &matrix_C.num_rows,
        &matrix_C.num_columns,
        &matrix_C.row_start,
        &row_end,
        &matrix_C.columns,
        &matrix_C.values
    );

    matrix_C.num_values = matrix_C.row_start[matrix_C.num_rows];
    mkl_sparse_destroy(mkl_matrix_C);

    state.matrix_C = matrix_C;

    return;
}

CPUGEMMRunnerState create_spgemm_mkl_state(std::string matrix_file_path) {
    crs_matrix_f64 matrix_A = load_mtx_file_into_crs(matrix_file_path, NULL);
    crs_matrix_f64 matrix_B = load_mtx_file_into_crs(matrix_file_path, NULL);

    sparse_matrix_t mkl_matrix_A;
    sparse_matrix_t mkl_matrix_B;

    mkl_sparse_d_create_csr(
        &mkl_matrix_A, 
        SPARSE_INDEX_BASE_ZERO, 
        matrix_A.num_rows, 
        matrix_A.num_columns, 
        matrix_A.row_start, 
        matrix_A.row_start + 1, 
        matrix_A.columns, 
        matrix_A.values
    );

    mkl_sparse_d_create_csr(
        &mkl_matrix_B, 
        SPARSE_INDEX_BASE_ZERO, 
        matrix_B.num_rows, 
        matrix_B.num_columns, 
        matrix_B.row_start, 
        matrix_B.row_start + 1, 
        matrix_B.columns, 
        matrix_B.values
    );

    return {
        .type = KernelType::SPGEMM__,
        .implementation = KernelImplementation::MKL__,
        .matrix_A = matrix_A, 
        .matrix_B = matrix_B,
        .mkl_matrix_A = mkl_matrix_A,
        .mkl_matrix_B = mkl_matrix_B
    };
}