#include <mkl.h>
#include <iostream>

#include "cpu_gemm_runner.hpp"
#include "matrix.hpp"
#include "mkl_kernels.hpp"
#include "custom_kernels.hpp"

/*****************************************************************************/
/******* kernel inspection and execution entrypoints *************************/
/*****************************************************************************/

void _run_inspection(CPUGEMMRunnerState &state, KernelType type, KernelImplementation implementation) {
    // dispatch the appropiate kernel inspection phase
         if ( type == KernelType::SPGEMM__ && implementation == KernelImplementation::MKL__ )    run_spgemm_mkl_inspection(state);
    else if ( type == KernelType::SPGEMM__ && implementation == KernelImplementation::CUSTOM__ ) run_spgemm_custom_inspection(state);
    else {
        std::cerr << "kernel not implemented for type: " << type << " and implementation: " << implementation << std::endl;
        exit(1);
    }
}

void _run_execution(CPUGEMMRunnerState &state, KernelType type, KernelImplementation implementation) {
    // dispatch the appropiate kernel execution phase
         if ( type == KernelType::SPGEMM__ && implementation == KernelImplementation::MKL__ )    run_spgemm_mkl_execution(state);
    else if ( type == KernelType::SPGEMM__ && implementation == KernelImplementation::CUSTOM__ ) run_spgemm_custom_execution(state);
    else {
        std::cerr << "kernel not implemented for type: " << type << " and implementation: " << implementation << std::endl;
        exit(1);
    }
}

void run_kernel(CPUGEMMRunnerState &state, KernelType type, KernelImplementation implementation) {
    _run_inspection(state, type, implementation);
    _run_execution(state, type, implementation);
}




/*****************************************************************************/
/******* runner state initialization *****************************************/
/*****************************************************************************/

CPUGEMMRunnerState create_runner_state(KernelType type, KernelImplementation implementation, const std::string &matrix_file_path) {
    // dispatch the appropiate runner initialization 
         if ( type == KernelType::SPGEMM__ && implementation == KernelImplementation::MKL__ )    return create_spgemm_mkl_state(matrix_file_path);
    else if ( type == KernelType::SPGEMM__ && implementation == KernelImplementation::CUSTOM__ ) return create_spgemm_custom_state(matrix_file_path);
    else {
        std::cerr << "kernel not implemented for type: " << type << " and implementation: " << implementation << std::endl;
        exit(1);
    }
}