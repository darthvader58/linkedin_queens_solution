#include "validator.h"
#include <stdio.h>
#include <stdlib.h>

// 8-directional diagonal neighbors
static const int DX[] = {-1, -1, -1, 0, 0, 1, 1, 1};
static const int DY[] = {-1, 0, 1, -1, 1, -1, 0, 1};

bool validator_is_diagonally_adjacent(const Puzzle* puzzle, int row1, int col1, int row2, int col2) {
    (void)puzzle; // Unused parameter
    int dr = abs(row1 - row2);
    int dc = abs(col1 - col2);
    
    // Adjacent if difference is at most 1 in both dimensions
    return (dr <= 1 && dc <= 1 && !(dr == 0 && dc == 0));
}

bool validator_has_adjacent_queen(const Puzzle* puzzle, int row, int col) {
    int n = puzzle_get_size(puzzle);
    
    for (int d = 0; d < 8; d++) {
        int nr = row + DX[d];
        int nc = col + DY[d];
        
        if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
            if (puzzle_get_cell(puzzle, nr, nc) >= 0) {
                return true;
            }
        }
    }
    
    return false;
}

bool validator_can_place(const Puzzle* puzzle, int row, int col, int region) {
    // Check if row is already used
    if (puzzle_is_row_used(puzzle, row)) {
        return false;
    }
    
    // Check if column is already used
    if (puzzle_is_col_used(puzzle, col)) {
        return false;
    }
    
    // Check if cell belongs to the target region
    if (puzzle_get_region(puzzle, row, col) != region) {
        return false;
    }
    
    // Check diagonal adjacency
    if (validator_has_adjacent_queen(puzzle, row, col)) {
        return false;
    }
    
    return true;
}

bool validator_check_rows(const Puzzle* puzzle) {
    int n = puzzle_get_size(puzzle);
    int queens_in_row[MAX_N] = {0};
    
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (puzzle->solution_board[row][col] >= 0) {
                queens_in_row[row]++;
            }
        }
    }
    
    for (int row = 0; row < n; row++) {
        if (queens_in_row[row] != 1) {
            printf("✗ Row %d has %d queens (expected 1)\n", row, queens_in_row[row]);
            return false;
        }
    }
    
    return true;
}

bool validator_check_columns(const Puzzle* puzzle) {
    int n = puzzle_get_size(puzzle);
    int queens_in_col[MAX_N] = {0};
    
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (puzzle->solution_board[row][col] >= 0) {
                queens_in_col[col]++;
            }
        }
    }
    
    for (int col = 0; col < n; col++) {
        if (queens_in_col[col] != 1) {
            printf("✗ Column %d has %d queens (expected 1)\n", col, queens_in_col[col]);
            return false;
        }
    }
    
    return true;
}

bool validator_check_regions(const Puzzle* puzzle) {
    int n = puzzle_get_size(puzzle);
    int queens_in_region[MAX_N] = {0};
    
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (puzzle->solution_board[row][col] >= 0) {
                int region = puzzle_get_region(puzzle, row, col);
                queens_in_region[region]++;
            }
        }
    }
    
    for (int region = 0; region < n; region++) {
        if (queens_in_region[region] != 1) {
            printf("✗ Region %d has %d queens (expected 1)\n", region, queens_in_region[region]);
            return false;
        }
    }
    
    return true;
}

bool validator_check_diagonals(const Puzzle* puzzle) {
    int n = puzzle_get_size(puzzle);
    
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (puzzle->solution_board[row][col] >= 0) {
                // Check all 8 neighbors
                for (int d = 0; d < 8; d++) {
                    int nr = row + DX[d];
                    int nc = col + DY[d];
                    
                    if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
                        if (puzzle->solution_board[nr][nc] >= 0) {
                            printf("✗ Queens diagonally adjacent at (%d,%d) and (%d,%d)\n",
                                   row, col, nr, nc);
                            return false;
                        }
                    }
                }
            }
        }
    }
    
    return true;
}

bool validator_verify_solution(const Puzzle* puzzle) {
    if (!puzzle_has_solution(puzzle)) {
        return false;
    }
    
    return validator_check_rows(puzzle) &&
           validator_check_columns(puzzle) &&
           validator_check_regions(puzzle) &&
           validator_check_diagonals(puzzle);
}