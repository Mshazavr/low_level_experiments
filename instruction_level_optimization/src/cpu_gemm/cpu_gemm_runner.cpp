#include <mkl.h>
#include <iostream>

#include "cpu_gemm_runner.hpp"
#include "matrix.hpp"
#include "mkl_kernels.hpp"
#include "custom_kernels.hpp"
#include "naive_kernels.hpp"

/*****************************************************************************/
/******* kernel inspection and execution entrypoints *************************/
/*****************************************************************************/

void run_inspection(CPUGEMMRunnerState &state) {
    // dispatch the appropiate kernel inspection phase
         if ( state.type == KernelType::SPGEMM__ && state.implementation == KernelImplementation::MKL__    ) run_spgemm_mkl_inspection(state);
    else if ( state.type == KernelType::SPGEMM__ && state.implementation == KernelImplementation::CUSTOM__ ) run_spgemm_custom_inspection(state);
    else if ( state.type == KernelType::SPMV__   && state.implementation == KernelImplementation::MKL__    ) run_spmv_mkl_inspection(state);
    else if ( state.type == KernelType::SPMV__   && state.implementation == KernelImplementation::CUSTOM__ ) run_spmv_custom_inspection(state);
    else if ( state.type == KernelType::SPMV__   && state.implementation == KernelImplementation::NAIVE__  ) run_spmv_naive_inspection(state);
    else if ( state.type == KernelType::SPMM__   && state.implementation == KernelImplementation::MKL__    ) run_spmm_mkl_inspection(state);
    else {
        std::cerr << "kernel not implemented for type: " << state.type << " and implementation: " << state.implementation << std::endl;
        exit(1);
    }
}

void run_execution(CPUGEMMRunnerState &state) {
    // dispatch the appropiate kernel execution phase
         if ( state.type == KernelType::SPGEMM__ && state.implementation == KernelImplementation::MKL__    ) run_spgemm_mkl_execution(state);
    else if ( state.type == KernelType::SPGEMM__ && state.implementation == KernelImplementation::CUSTOM__ ) run_spgemm_custom_execution(state);
    else if ( state.type == KernelType::SPMV__   && state.implementation == KernelImplementation::MKL__    ) run_spmv_mkl_execution(state);
    else if ( state.type == KernelType::SPMV__   && state.implementation == KernelImplementation::CUSTOM__ ) run_spmv_custom_execution(state);
    else if ( state.type == KernelType::SPMV__   && state.implementation == KernelImplementation::NAIVE__  ) run_spmv_naive_execution(state);
    else if ( state.type == KernelType::SPMM__   && state.implementation == KernelImplementation::MKL__    ) run_spmm_mkl_execution(state);
    else {
        std::cerr << "kernel not implemented for type: " << state.type << " and implementation: " << state.implementation << std::endl;
        exit(1);
    }
}

void run_kernel(CPUGEMMRunnerState &state) {
    run_inspection(state);
    run_execution(state);
}




/*****************************************************************************/
/******* runner state initialization and destruction *************************/
/*****************************************************************************/

CPUGEMMRunnerState create_runner_state(KernelType type, KernelImplementation implementation, const std::string &matrix_file_path) {
    // dispatch the appropiate runner initialization 
         if ( type == KernelType::SPGEMM__ && implementation == KernelImplementation::MKL__    ) return create_spgemm_mkl_state(matrix_file_path);
    else if ( type == KernelType::SPGEMM__ && implementation == KernelImplementation::CUSTOM__ ) return create_spgemm_custom_state(matrix_file_path);
    else if ( type == KernelType::SPMV__   && implementation == KernelImplementation::MKL__    ) return create_spmv_mkl_state(matrix_file_path);
    else if ( type == KernelType::SPMV__   && implementation == KernelImplementation::CUSTOM__ ) return create_spmv_custom_state(matrix_file_path);
    else if ( type == KernelType::SPMV__   && implementation == KernelImplementation::NAIVE__  ) return create_spmv_naive_state(matrix_file_path);
    else if ( type == KernelType::SPMM__   && implementation == KernelImplementation::MKL__    ) return create_spmm_mkl_state(matrix_file_path);
    else {
        std::cerr << "kernel not implemented for type: " << type << " and implementation: " << implementation << std::endl;
        exit(1);
    }
}

void destroy_runner_state(CPUGEMMRunnerState &state) {
    destroy_crs_matrix(state.sparse_matrix_A);
    destroy_ccs_matrix(state.sparse_ccs_matrix_A);
    destroy_crs_matrix(state.sparse_matrix_B);

    if (state.implementation == KernelImplementation::MKL__) {
        // the output CRS matrix just points to MKL internal memory
        mkl_sparse_destroy(state.sparse_mkl_matrix_C);
    }
    else {
        destroy_crs_matrix(state.sparse_matrix_C);
    }

    destroy_dense_matrix(state.dense_matrix_A);
    destroy_dense_matrix(state.dense_matrix_B);
    mkl_sparse_destroy(state.sparse_mkl_matrix_A);
    mkl_sparse_destroy(state.sparse_mkl_matrix_B);
    free(state.vector_b);
    free(state.vector_c);
    free(state.ccs_to_crs_map);
}