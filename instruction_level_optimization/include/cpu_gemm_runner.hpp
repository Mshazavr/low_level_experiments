#pragma once

#include <string>
#include <mkl.h>

#include "matrix.hpp"

struct CPUGEMMRunnerState {
    // The left sparse or dense matrix in the product
    crs_matrix_f64 *matrix_A;

    // The right sparse or dense matix in the product
    crs_matrix_f64 *matrix_B;

    // The vector in the matrix-vector product 
    double *vector_b; 

    // handles used by mkl for matrix-matrix products
    sparse_matrix_t mkl_matrix_A;
    sparse_matrix_t mkl_matrix_B;

    int32_t inspection_time_ms;
    int32_t execution_time_ms;
};

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


enum KernelImplementation {
    CUSTOM__ = 0,
    MKL__
};


void run_kernel(CPUGEMMRunnerState &state, KernelType type, KernelImplementation implementation);

CPUGEMMRunnerState create_runner_state(KernelType type, KernelImplementation implementation, const std::string &file_path);