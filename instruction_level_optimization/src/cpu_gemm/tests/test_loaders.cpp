#include <assert.h>

#include "matrix.hpp"
#include "arena_allocator.hpp"
#include "debug.hpp"

bool test_load_mtx_to_crs() {
    Arena arena = arena_create(10000000, 32);

    std::string file_path = "data/small.mtx";
    crs_matrix_f64 matrix = load_mtx_file_into_crs(file_path, arena);

    assert(matrix.num_rows == 4 && matrix.num_columns == 4 && matrix.num_values == 5);

    assert(matrix.values[0] == 2);
    assert(matrix.values[1] == 1);
    assert(matrix.values[2] == 1);
    assert(matrix.values[3] == 4);
    assert(matrix.values[4] == 5);

    assert(matrix.columns[0] == 1);
    assert(matrix.columns[1] == 0);
    assert(matrix.columns[2] == 2);
    assert(matrix.columns[3] == 2);
    assert(matrix.columns[4] == 3);

    assert(matrix.row_start[0] == 0);
    assert(matrix.row_start[1] == 1);
    assert(matrix.row_start[2] == 3);
    assert(matrix.row_start[3] == 3);
    assert(matrix.row_start[4] == 5);

    arena_destroy(arena);
    
    return true;
}

int main () {
    _log_test_result("test_load_mtx_to_crs", test_load_mtx_to_crs());
}