#include <type_traits>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <string>

#include "matrix.hpp"
#include "debug.hpp"


CRSMatrix _graph_to_crs(GraphMatrix &matrix, Arena &arena) {
    double  *values    =  (double *)arena_alloc_aligned(arena, matrix.num_values       * sizeof(double),  alignof(double));
    int32_t *columns   = (int32_t *)arena_alloc_aligned(arena, matrix.num_values       * sizeof(int32_t), alignof(int32_t));
    int32_t *row_start = (int32_t *)arena_alloc_aligned(arena, ( matrix.num_rows + 1 ) * sizeof(int32_t), alignof(int32_t));

    // double  *values    =  (double *)malloc(matrix.num_values       * sizeof(double));
    // int32_t *columns   = (int32_t *)malloc(matrix.num_values       * sizeof(int32_t));
    // int32_t *row_start = (int32_t *)malloc(( matrix.num_rows + 1 ) * sizeof(int32_t));

    std::vector< int32_t> indices(matrix.num_values);
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
    row_start[matrix.num_rows] = matrix.num_values;

    return {
        .num_rows = matrix.num_rows,
        .num_columns = matrix.num_columns,
        .values = values,
        .columns = columns,
        .row_start = row_start,
        .num_values = matrix.num_values
    };
}


GraphMatrix load_mtx_file_into_graph(const std::string &file_path, Arena &arena) {
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

            values  =  (double *)arena_alloc_aligned(arena, num_values * sizeof(double), alignof(double));
            rows    = (int32_t *)arena_alloc_aligned(arena, num_values * sizeof(int32_t), alignof(int32_t));
            columns = (int32_t *)arena_alloc_aligned(arena, num_values * sizeof(int32_t), alignof(int32_t));

            // values  =  (double *)malloc(num_values * sizeof(double));
            // rows    = (int32_t *)malloc(num_values * sizeof(int32_t));
            // columns = (int32_t *)malloc(num_values * sizeof(int32_t));
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


CRSMatrix load_mtx_file_into_crs(const std::string &file_path, Arena &arena) {
    _debug("loading %s into CRSMatrix", file_path.c_str());
    GraphMatrix graph_matrix = load_mtx_file_into_graph(file_path, arena);
    return _graph_to_crs(graph_matrix, arena);
}