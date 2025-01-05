#pragma once

#include <string>

#include "cpu_gemm_runner.hpp"

void run_spgemm_custom_inspection(CPUGEMMRunnerState &state);

void run_spgemm_custom_execution(CPUGEMMRunnerState &state);

CPUGEMMRunnerState create_spgemm_custom_state(std::string matrix_file_path);