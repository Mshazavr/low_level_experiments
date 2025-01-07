#pragma once

#include <string>

#include "cpu_gemm_runner.hpp"

void run_spgemm_mkl_inspection(CPUGEMMRunnerState &state);
void run_spgemm_mkl_execution(CPUGEMMRunnerState &state);
CPUGEMMRunnerState create_spgemm_mkl_state(std::string matrix_file_path);

void run_spmv_mkl_inspection(CPUGEMMRunnerState &state);
void run_spmv_mkl_execution(CPUGEMMRunnerState &state);
CPUGEMMRunnerState create_spmv_mkl_state(std::string matrix_file_path);


void run_spmm_mkl_inspection(CPUGEMMRunnerState &state);
void run_spmm_mkl_execution(CPUGEMMRunnerState &state);
CPUGEMMRunnerState create_spmm_mkl_state(std::string matrix_file_path);