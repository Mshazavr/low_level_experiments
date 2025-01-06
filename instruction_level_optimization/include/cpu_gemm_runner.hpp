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


enum KernelImplementation {
    CUSTOM__ = 0,
    MKL__
};


struct CPUGEMMRunnerState {
    // kernel type and implementation type 
    KernelType type;
    KernelImplementation implementation;

    // the left sparse or dense matrix in the product
    crs_matrix_f64 matrix_A;

    // the right sparse or dense matix in the product
    crs_matrix_f64 matrix_B;

    // the vector in the matrix-vector product 
    double *vector_b; 

    // handles used by mkl for matrix-matrix products
    sparse_matrix_t mkl_matrix_A;
    sparse_matrix_t mkl_matrix_B;

    // sparse or dense result matrix
    crs_matrix_f64 matrix_C;

    int32_t inspection_time_ms;
    int32_t execution_time_ms;
};

void run_kernel(CPUGEMMRunnerState &state);

CPUGEMMRunnerState create_runner_state(KernelType type, KernelImplementation implementation, const std::string &file_path);