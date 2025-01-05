#pragma once

#include <cstdint>
#include <string>

#include "arena_allocator.hpp"


/*
Basic matrix storage formats

A Detailed description for each format is provided 
in the docstring of the corresponding struct

DENSE = dense 
GRAPH = graph
CRS   = compressed row storage 
CCR   = compressed column storage 
BCRS  = block compressed row storage 
BCCS  = block compressed column storage
*/
enum MatrixFormat {
    DENSE = 0,
    GRAPH,
    CRS,
    CCS,
    BCRS,
    BCCS
};

/*
Dense storage format for regular matrices
*/
struct DenseMatrix {
    int32_t num_rows, num_columns;
    double *values;

    size_t mem_size_b() {
        return num_rows * num_columns * sizeof(double);
    }
};


/*
Graph storage format for regular matrices
The order of the values is arbitrary. This storage
format can be used for custom permutations

The format is called graph since each non-zero value
can be thought of as an edge in the bi-partite graph
connecting the rows to columns

Example:
For the matrix A = [
    [0, 2, 0, 0]
    [1, 0, 1, 0]
    [0, 0, 0, 0]
    [0, 0, 4, 5]
]
values  = [2, 1, 1, 4, 5]
rows    = [0, 1, 1, 3, 3]
columns = [1, 0, 3, 2, 3]
*/
struct GraphMatrix {
    int32_t num_rows, num_columns;
    double *values;
    int32_t *rows;
    int32_t *columns;
    int32_t num_values;

    size_t mem_size_b() {
        return num_values * (sizeof(double) + 2 * sizeof(int32_t));
    }
};

/*
Compressed-row-storage (CRS) format for sparse matrices

Example:
For the matrix A = [
    [0, 2, 0, 0]
    [1, 0, 1, 0]
    [0, 0, 0, 0]
    [0, 0, 4, 5]
]
values     = [2, 1, 1, 4, 5]
columns    = [1, 0, 2, 2, 3]
row_start  = [0, 1, 3, 3]

Note that row_start indexes the values array
*/
struct CRSMatrix {
    int32_t num_rows, num_columns;
    double *values; 
    int32_t *columns; 
    int32_t *row_start;
    int32_t num_values; 

    size_t mem_size_b() {
        return (
            num_values * (sizeof(double) + sizeof(int32_t)) + 
            num_rows * sizeof(int32_t)
        );
    }
};


/*
Compressed-column-storage (CCS) format for sparse matrices

Example:
For the matrix A = [
    [0, 2, 0, 0]
    [1, 0, 1, 0]
    [0, 0, 0, 0]
    [0, 0, 4, 5]
]
values        = [1, 2, 1, 4, 5]
rows          = [1, 0, 1, 3, 3]
column_start  = [0, 1, 2, 4]

Note that column_start indexes the values array
*/
struct CCSMatrix {
    int32_t num_rows, num_columns;
    double *values; 
    int32_t *rows; 
    int32_t *column_start;
    int32_t num_values; 

    size_t mem_size_b() {
        return (
            num_values * (sizeof(double) + sizeof(int32_t)) + 
            num_columns * sizeof(int32_t)
        );
    }
};


/*
Block-compressed-row-storage (BCRS) format for sparse matrices

*The matrix is required to be padded with 0 columns and rows to 
ensure the row and column dimensions are divisible by block 
height and width respectively

Example:
For the matrix A = [
    [0, 2, 3, 0]
    [1, 0, 1, 0]
    [0, 0, 0, 0]
    [0, 0, 4, 5]
] with block dimensions (2, 2)
values        = [>0, 2, 1, 0<, >3, 0, 1, 0<, >0, 0, 4, 5<] 
columns       = [0, 1, 1]
row_start     = [0, 2]

(blocks in values are highlighted with ><)

Note that row_start indexes the values array
*/
struct BCRSMatrix {
    int32_t num_rows, num_columns;
    int32_t block_height, block_width;
    double *values; 
    int32_t *columns; 
    int32_t *row_start;
    int32_t num_values; 

    size_t mem_size_b() {
        return (
            num_values * sizeof(double) +
            (num_values / (block_height * block_width)) * sizeof(int32_t) + 
            (num_rows / block_height) * sizeof(int32_t)
        );
    }
};


/*
Block-compressed-column-storage (BCCS) format for sparse matrices

*The matrix is required to be padded with 0 columns and rows to 
ensure the row and column dimensions are divisible by block 
height and width respectively

Example:
For the matrix A = [
    [0, 2, 3, 0]
    [1, 0, 1, 0]
    [0, 0, 0, 0]
    [0, 0, 4, 5]
] with block dimensions (2, 2)
values           = [>0, 2, 1, 0<, >3, 0, 1, 0<, >0, 0, 4, 5<] 
rows             = [0, 0, 1]
column_start     = [0, 1]

(blocks in values are highlighted with ><)

Note that column_start indexes the values array
*/
struct BCCSMatrix {
    int32_t num_rows, num_columns;
    int32_t block_height, block_width;
    double *values; 
    int32_t *rows; 
    int32_t *column_start;
    int32_t num_values;

    size_t mem_size_b() {
        return (
            num_values * sizeof(double) +
            (num_values / (block_height * block_width)) * sizeof(int32_t) + 
            (num_columns / block_width) * sizeof(int32_t)
        );
    }
};

typedef DenseMatrix dense_matrix_f64;
typedef GraphMatrix graph_matrix_f64;
typedef CRSMatrix   crs_matrix_f64;
typedef CCSMatrix   ccs_matrix_f64;
typedef BCRSMatrix  bcrs_matrix_f64;
typedef BCCSMatrix  bccs_matrix_f64;




CRSMatrix _graph_to_crs(GraphMatrix &matrix, Arena &arena);

/*
Entrypoint functions for loading .mtx files (matrix market file) 
into different formats
*/
GraphMatrix load_mtx_file_into_graph(const std::string &file_path, Arena &arena);
CRSMatrix load_mtx_file_into_crs(const std::string &file_path, Arena &arena);