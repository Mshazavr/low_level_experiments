cmake_minimum_required(VERSION 3.10) 
project(InstructionLevelOptimization)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -march=native -DDEBUG")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(SOURCES
    src/reduce/reduce.cpp
    src/reduce/reduce_threaded.cpp
    src/cache_experiment/single_threaded_shuffle.cpp
    src/run_info.cpp 
    src/utils/machine_info.cpp
    src/utils/debug.cpp
    src/utils/timer.cpp
)

set(HEADERS
    include/reduce.hpp
    include/reduce_threaded.hpp
    include/single_threaded_shuffle.hpp
    include/run_info.hpp
    include/sum_mode.hpp
    include/machine_info.hpp
    include/debug.hpp
    include/timer.hpp
)

add_executable(reduce_runner src/reduce/main.cpp ${SOURCES} ${HEADERS})
add_executable(cache_experiment_runner src/cache_experiment/main.cpp ${SOURCES} ${HEADERS})

target_include_directories(reduce_runner PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(cache_experiment_runner PRIVATE ${CMAKE_SOURCE_DIR}/include)