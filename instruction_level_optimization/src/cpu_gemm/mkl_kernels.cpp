#include <string>
#include <mkl.h>
#include <mkl_spblas.h>

#include "mkl_kernels.hpp"
#include "cpu_gemm_runner.hpp"
#include "timer.hpp"


/****************************************************************************/
/**** SPGEMM ****************************************************************/
/****************************************************************************/

void run_spgemm_mkl_inspection(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();
    // TODO: 
    // if test cases include special matrices (e.g. symmetric)
    // add the best faith available optimization for mkl for the specific matrix type
    int32_t end_ms = get_microseconds();
    state.inspection_time_ms = end_ms - start_ms;
    return;
}

void run_spgemm_mkl_execution(CPUGEMMRunnerState &state) {
    mkl_set_num_threads(1); // TODO: parametrize threads
    int32_t start_ms = get_microseconds();

    sparse_matrix_t mkl_matrix_C;

    sparse_status_t status = mkl_sparse_spmm(
        SPARSE_OPERATION_NON_TRANSPOSE, 
        state.sparse_mkl_matrix_A, 
        state.sparse_mkl_matrix_B, 
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

    state.sparse_matrix_C = matrix_C;
    state.sparse_mkl_matrix_C = mkl_matrix_C;

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
        .sparse_matrix_A = matrix_A, 
        .sparse_matrix_B = matrix_B,
        .sparse_mkl_matrix_A = mkl_matrix_A,
        .sparse_mkl_matrix_B = mkl_matrix_B
    };
}


/****************************************************************************/
/**** SPMV ******************************************************************/
/****************************************************************************/

void run_spmv_mkl_inspection(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();
    //...
    int32_t end_ms = get_microseconds();
    state.inspection_time_ms = end_ms - start_ms;;
}

void run_spmv_mkl_execution(CPUGEMMRunnerState &state) {
    mkl_set_num_threads(1); // TODO: parametrize threads
    int32_t start_ms = get_microseconds();

    state.vector_c = (double *)malloc(state.sparse_matrix_A.num_rows * sizeof(double));

    struct matrix_descr descriptor = {
        .type=SPARSE_MATRIX_TYPE_GENERAL,
        .mode=SPARSE_FILL_MODE_FULL,
        .diag=SPARSE_DIAG_NON_UNIT
    };

    sparse_status_t status = mkl_sparse_d_mv(
        SPARSE_OPERATION_NON_TRANSPOSE,
        1.0, // alpha
        state.sparse_mkl_matrix_A,
        descriptor,
        state.vector_b, 
        0.0, // beta
        state.vector_c 
    );

    int32_t end_ms = get_microseconds();
    state.execution_time_ms = end_ms - start_ms;
    return;
}

CPUGEMMRunnerState create_spmv_mkl_state(std::string matrix_file_path) {
    crs_matrix_f64 matrix_A = load_mtx_file_into_crs(matrix_file_path, NULL);
    
    sparse_matrix_t mkl_matrix_A;

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

    double *vector_b = (double *)malloc(matrix_A.num_columns * sizeof(double));
    for(size_t i = 0; i < matrix_A.num_columns; ++i) {
        vector_b[i] = (double)(std::rand() / (RAND_MAX / 100));
    }

    return {
        .type = KernelType::SPMV__,
        .implementation = KernelImplementation::MKL__,
        .sparse_matrix_A = matrix_A,
        .vector_b = vector_b,
        .sparse_mkl_matrix_A = mkl_matrix_A
    };
}



/****************************************************************************/
/**** SPMM ******************************************************************/
/****************************************************************************/

void run_spmm_mkl_inspection(CPUGEMMRunnerState &state) {
    int32_t start_ms = get_microseconds();
    //...
    int32_t end_ms = get_microseconds();
    state.inspection_time_ms = end_ms - start_ms;;
}

void run_spmm_mkl_execution(CPUGEMMRunnerState &state) {
    mkl_set_num_threads(1); // TODO: parametrize threads
    int32_t start_ms = get_microseconds();

    state.dense_matrix_C = {
        .num_rows = state.sparse_matrix_A.num_rows,
        .num_columns = state.dense_matrix_B.num_columns,
        .values = (double *)malloc(state.sparse_matrix_A.num_rows * state.dense_matrix_B.num_columns * sizeof(double)),
        .order = MatrixOrder::COLUMN_MAJOR
    };

    struct matrix_descr descriptor = {
        .type=SPARSE_MATRIX_TYPE_GENERAL,
        .mode=SPARSE_FILL_MODE_FULL,
        .diag=SPARSE_DIAG_NON_UNIT
    };

    sparse_status_t status = mkl_sparse_d_mm(
        SPARSE_OPERATION_NON_TRANSPOSE,
        1.0, // alpha
        state.sparse_mkl_matrix_A,
        descriptor,
        SPARSE_LAYOUT_COLUMN_MAJOR,
        state.dense_matrix_B.values,
        state.dense_matrix_B.num_columns,
        state.dense_matrix_B.num_rows,
        0.0, // beta
        state.dense_matrix_C.values,
        state.dense_matrix_C.num_rows
    );

    int32_t end_ms = get_microseconds();
    state.execution_time_ms = end_ms - start_ms;
    return;
}

CPUGEMMRunnerState create_spmm_mkl_state(std::string matrix_file_path) {
    crs_matrix_f64 matrix_A = load_mtx_file_into_crs(matrix_file_path, NULL);
    
    sparse_matrix_t mkl_matrix_A;

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

    dense_matrix_f64 matrix_B = {
        .num_rows = matrix_A.num_columns,
        .num_columns = 100, // TODO: parametrize this
        .values = (double *)malloc(matrix_A.num_columns * 100 * sizeof(double)),
        .order = MatrixOrder::COLUMN_MAJOR
    };
    for(size_t i = 0; i < matrix_B.num_rows * matrix_B.num_columns; ++i) {
        matrix_B.values[i] = (double)(std::rand() / (RAND_MAX / 100));
    }

    return {
        .type = KernelType::SPMV__,
        .implementation = KernelImplementation::MKL__,
        .sparse_matrix_A = matrix_A,
        .dense_matrix_B = matrix_B,
        .sparse_mkl_matrix_A = mkl_matrix_A
    };
}