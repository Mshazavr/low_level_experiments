#pragma once

#include <string>
#include <mkl.h>

#include "matrix.hpp"

enum KernelType {
    // dense matrix * vector
    DGEMV__ = 0,
    
    // dense matrix * dense matrix
    DGEMM__,

    // sparse matrix * vector
    SPMV__, 

    // sparse matrix * dense matrix
    SPMM__,

    // sparse matrix * sparse matrix
    SPGEMM__
};
inline constexpr std::string_view KernelTypeNames[] = {
    "DGEMV", "DGEMM", "SPMV", "SPMM", "SPGEMM"
};


enum KernelImplementation {
    NAIVE__ = 0,
    MKL__,
    CUSTOM__
};
inline constexpr std::string_view KernelImplementationNames[] = {
    "NAIVE", "MKL", "CUSTOM"
};


struct CPUGEMMRunnerState {
    // kernel type and implementation type 
    KernelType type;
    KernelImplementation implementation;

    // the left sparse or dense matrix in the product
    crs_matrix_f64 sparse_matrix_A;
    ccs_matrix_f64 sparse_ccs_matrix_A;
    dense_matrix_f64 dense_matrix_A;

    // the right sparse or dense matix in the product
    crs_matrix_f64 sparse_matrix_B;
    dense_matrix_f64 dense_matrix_B;

    // the vector in the matrix-vector product 
    double *vector_b; 

    // sparse or dense result matrix
    crs_matrix_f64 sparse_matrix_C;
    dense_matrix_f64 dense_matrix_C;

    // result vector 
    double *vector_c;

    // handles used by mkl for the sparse matrices
    sparse_matrix_t sparse_mkl_matrix_A;
    sparse_matrix_t sparse_mkl_matrix_B;
    sparse_matrix_t sparse_mkl_matrix_C;

    // ccs to crs permutation map
    // can be computed during inspection
    int32_t *ccs_to_crs_map;

    int32_t inspection_time_ms;
    int32_t execution_time_ms;
    int32_t ticks[10]; // useful for timing different portions of the kernel
};

void run_inspection(CPUGEMMRunnerState &state);
void run_execution(CPUGEMMRunnerState &state);
void run_kernel(CPUGEMMRunnerState &state);

CPUGEMMRunnerState create_runner_state(KernelType type, KernelImplementation implementation, const std::string &file_path);

void destroy_runner_state(CPUGEMMRunnerState &state);