

#include <iostream>
#include <mkl.h>
#include <mkl_spblas.h>

#include "arena_allocator.hpp"
#include "matrix.hpp"

using namespace std;


crs_matrix_f64 spmm_mkl(const crs_matrix_f64 &A, const crs_matrix_f64 &B) {
    sparse_matrix_t A_handle, B_handle, C_handle;
    mkl_set_num_threads(1);

    mkl_sparse_d_create_csr(
        &A_handle, 
        SPARSE_INDEX_BASE_ZERO, 
        A.num_rows, A.num_columns, 
        A.row_start, A.row_start + 1, A.columns, A.values
    );


    mkl_sparse_d_create_csr(
        &B_handle, 
        SPARSE_INDEX_BASE_ZERO, 
        B.num_rows, B.num_columns, 
        B.row_start, B.row_start + 1, B.columns, B.values
    );
    
    mkl_sparse_spmm(SPARSE_OPERATION_NON_TRANSPOSE, A_handle, B_handle, &C_handle);

    crs_matrix_f64 C = {};
        sparse_index_base_t indexing;

    int32_t *row_end = C.row_start + 1;

    mkl_sparse_d_export_csr(
        C_handle, &indexing, 
        &C.num_rows,
        &C.num_columns,
        &C.row_start,
        &row_end,
        &C.columns,
        &C.values
    );

    C.num_values = C.row_start[C.num_rows];

    mkl_sparse_destroy(A_handle);
    mkl_sparse_destroy(B_handle);
    mkl_sparse_destroy(C_handle);

    return C;
}

int main () {
    Arena arena = arena_create(100000, 32);

    std::string file_path = "data/small.mtx";
    // std::string file_path = "data/bcsstk16.mtx";
    crs_matrix_f64 A = load_mtx_file_into_crs(file_path, arena);
    crs_matrix_f64 B = load_mtx_file_into_crs(file_path, arena);

    crs_matrix_f64 C = spmm_mkl(A, B);
    cout << C.num_rows << " " << C.num_columns << endl;
    cout << C.num_values << endl;
    for(int i = 0; i < 5; ++i) {
        cout << C.columns[i] << " " << C.values[i] << endl;
    }
    cout << C.row_start[0] << endl;
    cout << C.row_start[1] << endl;
    cout << C.row_start[2] << endl;
    cout << C.row_start[3] << endl;
    cout << C.row_start[4] << endl;

    arena_destroy(arena);
    return 0;
}
