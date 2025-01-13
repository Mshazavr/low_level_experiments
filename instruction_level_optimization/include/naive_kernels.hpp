#pragma once

#include <string>

#include "cpu_gemm_runner.hpp"

void run_spgemm_naive_inspection(CPUGEMMRunnerState &state);
void run_spgemm_naive_execution(CPUGEMMRunnerState &state);
CPUGEMMRunnerState create_spgemm_naive_state(std::string matrix_file_path);

void run_spmv_naive_inspection(CPUGEMMRunnerState &state);
void run_spmv_naive_execution(CPUGEMMRunnerState &state);
CPUGEMMRunnerState create_spmv_naive_state(std::string matrix_file_path);

void run_spmm_naive_inspection(CPUGEMMRunnerState &state);
void run_spmm_naive_execution(CPUGEMMRunnerState &state);
CPUGEMMRunnerState create_spmm_naive_state(std::string matrix_file_path);