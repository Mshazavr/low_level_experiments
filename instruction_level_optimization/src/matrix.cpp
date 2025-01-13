#include <type_traits>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <string>

#include "matrix.hpp"
#include "debug.hpp"

std::string get_file_type_from_name(std::string file_path) {
    if (file_path.substr(file_path.size() - 4) == ".mtx") {
        return ".mtx";
    }

    if (file_path.substr(file_path.size() - 5) == ".smtx") {
        return ".smtx";
    }

    std::cerr << "Invalid matrix file extension: " << file_path << std::endl;
    exit(1);
}

GraphMatrix _crs_to_graph(CRSMatrix &matrix, Arena *arena) {
    double *values;
    int32_t *rows;
    int32_t *columns;

    if (arena) {
        values    =  (double *)arena_alloc_aligned(*arena, matrix.num_values       * sizeof(double),  alignof(double));
        rows      = (int32_t *)arena_alloc_aligned(*arena, matrix.num_values       * sizeof(int32_t), alignof(int32_t));
        columns   = (int32_t *)arena_alloc_aligned(*arena, matrix.num_values       * sizeof(int32_t), alignof(int32_t));
    }
    else {
        values    =  (double *)malloc(matrix.num_values       * sizeof(double));
        rows      = (int32_t *)malloc(matrix.num_values       * sizeof(int32_t));
        columns   = (int32_t *)malloc(matrix.num_values       * sizeof(int32_t));
    }

    for (int32_t row = 0; row < matrix.num_rows; ++row) {
        for (int32_t ind = matrix.row_start[row]; ind < matrix.row_start[row+1]; ++ind) {
            values[ind]  = matrix.values[ind];
            rows[ind]    = row;
            columns[ind] = matrix.columns[ind];
        }
    }

    return {
        .num_rows = matrix.num_rows,
        .num_columns = matrix.num_columns,
        .values = values,
        .rows = rows,
        .columns = columns,
        .num_values = matrix.num_values
    };
}

CRSMatrix _graph_to_crs(GraphMatrix &matrix, Arena *arena) {
    double *values;
    int32_t *columns;
    int32_t *row_start;

    if (arena) {
        values    =  (double *)arena_alloc_aligned(*arena, matrix.num_values       * sizeof(double),  alignof(double));
        columns   = (int32_t *)arena_alloc_aligned(*arena, matrix.num_values       * sizeof(int32_t), alignof(int32_t));
        row_start = (int32_t *)arena_alloc_aligned(*arena, ( matrix.num_rows + 1 ) * sizeof(int32_t), alignof(int32_t));
    }
    else {
        values    =  (double *)malloc(matrix.num_values       * sizeof(double));
        columns   = (int32_t *)malloc(matrix.num_values       * sizeof(int32_t));
        row_start = (int32_t *)malloc(( matrix.num_rows + 1 ) * sizeof(int32_t));
    }

    std::vector<int32_t> indices(matrix.num_values);
    for(int32_t i = 0; i < matrix.num_values; ++i) {
        indices[i] = i;
    }

    auto sort_key = [&matrix](int32_t ind1, int32_t ind2) {
        return (
            std::make_pair(matrix.rows[ind1], matrix.columns[ind1]) < 
            std::make_pair(matrix.rows[ind2], matrix.columns[ind2])
        );
    };

    std::sort(indices.begin(), indices.end(), sort_key);
    uint32_t current_row = 0;
    row_start[current_row] = 0;
    for(int32_t i = 0; i < indices.size(); ++i) {
        int32_t matrix_idx = indices[i];

        double  value  = matrix.values[matrix_idx];
        int32_t column = matrix.columns[matrix_idx];
        int32_t row    = matrix.rows[matrix_idx];

        values[i]  = value;
        columns[i] = column;
        
        if (row > current_row) {
            for(int32_t j = current_row + 1; j <= row; ++j) {
                row_start[j] = i;
            }
            current_row = row;
        }
    }
    for (int32_t j = current_row + 1; j <= matrix.num_rows; ++j) {
        row_start[matrix.num_rows] = matrix.num_values;
    }

    return {
        .num_rows = matrix.num_rows,
        .num_columns = matrix.num_columns,
        .values = values,
        .columns = columns,
        .row_start = row_start,
        .num_values = matrix.num_values
    };
}


CCSMatrix _graph_to_ccs(GraphMatrix &matrix, Arena *arena) {
    double *values;
    int32_t *rows;
    int32_t *column_start;

    if (arena) {
        values       =  (double *)arena_alloc_aligned(*arena, matrix.num_values          * sizeof(double),  alignof(double));
        rows         = (int32_t *)arena_alloc_aligned(*arena, matrix.num_values          * sizeof(int32_t), alignof(int32_t));
        column_start = (int32_t *)arena_alloc_aligned(*arena, ( matrix.num_columns + 1 ) * sizeof(int32_t), alignof(int32_t));
    }
    else {
        values       =  (double *)malloc(matrix.num_values          * sizeof(double));
        rows         = (int32_t *)malloc(matrix.num_values          * sizeof(int32_t));
        column_start = (int32_t *)malloc(( matrix.num_columns + 1 ) * sizeof(int32_t));
    }

    std::vector<int32_t> indices(matrix.num_values);
    for(int32_t i = 0; i < matrix.num_values; ++i) {
        indices[i] = i;
    }

    auto sort_key = [&matrix](int32_t ind1, int32_t ind2) {
        return (
            std::make_pair(matrix.columns[ind1], matrix.rows[ind1]) < 
            std::make_pair(matrix.columns[ind2], matrix.rows[ind2])
        );
    };

    std::sort(indices.begin(), indices.end(), sort_key);
    uint32_t current_column = 0;
    column_start[current_column] = 0;
    for(int32_t i = 0; i < indices.size(); ++i) {
        int32_t matrix_idx = indices[i];

        double  value  = matrix.values[matrix_idx];
        int32_t column = matrix.columns[matrix_idx];
        int32_t row    = matrix.rows[matrix_idx];

        values[i]  = value;
        rows[i]    = row;
        
        if (column > current_column) {
            for(int32_t j = current_column + 1; j <= row; ++j) {
                column_start[j] = i;
            }
            current_column = row;
        }
    }
    for (int32_t j = current_column + 1; j <= matrix.num_rows; ++j) {
        column_start[matrix.num_rows] = matrix.num_values;
    }

    return {
        .num_rows = matrix.num_rows,
        .num_columns = matrix.num_columns,
        .values = values,
        .rows = rows,
        .column_start = column_start,
        .num_values = matrix.num_values
    };
}


GraphMatrix load_mtx_file_into_graph(const std::string &file_path, Arena *arena) {
    _debug("loading %s into GraphMatrix", file_path.c_str());
    std::ifstream file(file_path);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        exit(1);
    }

    int32_t num_rows, num_columns, num_values;
    double *values;
    int32_t *rows;
    int32_t *columns;

    std::string line;
    int32_t num_read = 0;

    while (std::getline(file, line)) {
        // skip headers
        if (line[0] == '%') {
            continue;
        }
        
        std::istringstream iss(line);
        if (num_read == 0) {
            iss >> num_rows >> num_columns >> num_values;

            if (arena) {
                values  =  (double *)arena_alloc_aligned(*arena, num_values * sizeof(double),  alignof(double));
                rows    = (int32_t *)arena_alloc_aligned(*arena, num_values * sizeof(int32_t), alignof(int32_t));
                columns = (int32_t *)arena_alloc_aligned(*arena, num_values * sizeof(int32_t), alignof(int32_t));
            }
            else {
                values  =  (double *)malloc(num_values * sizeof(double));
                rows    = (int32_t *)malloc(num_values * sizeof(int32_t));
                columns = (int32_t *)malloc(num_values * sizeof(int32_t));
            }
        } else {
            iss >> rows[num_read-1] >> columns[num_read-1] >> values[num_read-1];
            
            // go from 1-indexed to 0-indexed
            rows[num_read-1]--;
            columns[num_read-1]--;
        }
        num_read++;
    }

    return {
        .num_rows = num_rows,
        .num_columns = num_columns,
        .values = values,
        .rows = rows,
        .columns = columns,
        .num_values = num_values
    };
}


CRSMatrix load_smtx_file_into_crs(const std::string &file_path, Arena *arena) {
    _debug("loading %s into CRSMatrix", file_path.c_str());
    std::ifstream file(file_path);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        exit(1);
    }

    int32_t num_rows, num_columns, num_values;
    double *values;
    int32_t *columns;
    int32_t *row_start; 

    char comma;
    file >> num_rows >> comma >> num_columns >> comma >> num_values;
    if (arena) {
        values    =  (double *)arena_alloc_aligned(*arena, num_values       * sizeof(double),  alignof(double));
        columns   = (int32_t *)arena_alloc_aligned(*arena, num_values       * sizeof(int32_t), alignof(int32_t));
        row_start = (int32_t *)arena_alloc_aligned(*arena, ( num_rows + 1 ) * sizeof(int32_t), alignof(int32_t));
    }
    else {
        values    =  (double *)malloc(num_values       * sizeof(double));
        columns   = (int32_t *)malloc(num_values       * sizeof(int32_t));
        row_start = (int32_t *)malloc(( num_rows + 1 ) * sizeof(int32_t));
    }

    for (int32_t i = 0; i <= num_rows; ++i) {
        file >> row_start[i];
    }

    for (int32_t i = 0; i <= num_values; ++i) {
        file >> columns[i];
    }

    for (int32_t i = 0; i <= num_values; ++i) {
        values[i] = (double)(std::rand() / (RAND_MAX / 100));
    }
    
    return {
        .num_rows = num_rows,
        .num_columns = num_columns,
        .values = values,
        .columns = columns,
        .row_start = row_start,
        .num_values = num_values
    };
}


CRSMatrix load_mtx_file_into_crs(const std::string &file_path, Arena *arena) {
    _debug("loading %s into CRSMatrix", file_path.c_str());
    GraphMatrix graph_matrix = load_mtx_file_into_graph(file_path, NULL);
    CRSMatrix crs_matrix = _graph_to_crs(graph_matrix, arena);
    destroy_graph_matrix(graph_matrix);
    return crs_matrix;
}


CCSMatrix load_mtx_file_into_ccs(const std::string &file_path, Arena *arena) {
    _debug("loading %s into CRSMatrix", file_path.c_str());
    GraphMatrix graph_matrix = load_mtx_file_into_graph(file_path, NULL);
    CCSMatrix ccs_matrix = _graph_to_ccs(graph_matrix, arena);
    destroy_graph_matrix(graph_matrix);
    return ccs_matrix;
}


GraphMatrix load_file_into_graph(const std::string &file_path, Arena *arena) {
    std::string file_type = get_file_type_from_name(file_path);
    if (file_type == ".mtx") {
        return load_mtx_file_into_graph(file_path, arena);
    }
    else {
        CRSMatrix A = load_smtx_file_into_crs(file_path, NULL);
        GraphMatrix Result = _crs_to_graph(A, arena);
        destroy_crs_matrix(A);
        return Result;
    }
}

CRSMatrix load_file_into_crs(const std::string &file_path, Arena *arena) {
    std::string file_type = get_file_type_from_name(file_path);
    if (file_type == ".mtx") {
        return load_mtx_file_into_crs(file_path, arena);
    }
    else {
        return load_smtx_file_into_crs(file_path, arena);
    }
}

CCSMatrix load_file_into_ccs(const std::string &file_path, Arena *arena) {
    std::string file_type = get_file_type_from_name(file_path);
    if (file_type == ".mtx") {
        return load_mtx_file_into_ccs(file_path, arena);
    }
    else {
        CRSMatrix A = load_smtx_file_into_crs(file_path, NULL);
        GraphMatrix B = _crs_to_graph(A, NULL);
        CCSMatrix Result = _graph_to_ccs(B, arena);
        destroy_crs_matrix(A);
        destroy_graph_matrix(B);
        return Result;
    }
}



void destroy_crs_matrix(CRSMatrix &matrix) {
    free(matrix.columns);
    free(matrix.row_start);
    free(matrix.values);
}

void destroy_ccs_matrix(CCSMatrix &matrix) {
    free(matrix.rows);
    free(matrix.column_start);
    free(matrix.values);
}

void destroy_dense_matrix(DenseMatrix &matrix) {
    free(matrix.values);
}

void destroy_graph_matrix(GraphMatrix &matrix) {
    free(matrix.values);
    free(matrix.rows);
    free(matrix.columns);
}