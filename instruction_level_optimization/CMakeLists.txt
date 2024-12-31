cmake_minimum_required(VERSION 3.10) 
project(InstructionLevelOptimization)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -march=native -DDEBUG")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(SOURCES
    src/reduce.cpp
    src/reduce_threaded.cpp
    src/avx.cpp
    src/debug.cpp
    src/run_info.cpp 
    src/timer.cpp
)

set(HEADERS
    include/reduce.hpp
    include/reduce_threaded.hpp
    include/sum_mode.hpp
    include/avx.hpp
    include/debug.hpp
    include/avx.hpp
    include/run_info.hpp
    include/timer.hpp
)

add_executable(reduce_runner src/runner.cpp ${SOURCES} ${HEADERS})

target_include_directories(reduce_runner PRIVATE ${CMAKE_SOURCE_DIR}/include)