#include "puzzle.h"
#include <stdlib.h>
#include <string.h>

Puzzle* puzzle_create(int n) {
    Puzzle* puzzle = (Puzzle*)malloc(sizeof(Puzzle));
    if (puzzle) {
        puzzle_init(puzzle, n);
    }
    return puzzle;
}

void puzzle_destroy(Puzzle* puzzle) {
    if (puzzle) {
        free(puzzle);
    }
}

void puzzle_init(Puzzle* puzzle, int n) {
    puzzle->n = n;
    puzzle->solution_count = 0;
    puzzle->found = false;
    
    memset(puzzle->board, -1, sizeof(puzzle->board));
    memset(puzzle->solution_board, -1, sizeof(puzzle->solution_board));
    memset(puzzle->row_used, false, sizeof(puzzle->row_used));
    memset(puzzle->col_used, false, sizeof(puzzle->col_used));
}

int puzzle_get_size(const Puzzle* puzzle) {
    return puzzle->n;
}

int puzzle_get_cell(const Puzzle* puzzle, int row, int col) {
    if (row >= 0 && row < puzzle->n && col >= 0 && col < puzzle->n) {
        return puzzle->board[row][col];
    }
    return -1;
}

int puzzle_get_region(const Puzzle* puzzle, int row, int col) {
    if (row >= 0 && row < puzzle->n && col >= 0 && col < puzzle->n) {
        return puzzle->regions[row][col];
    }
    return -1;
}

bool puzzle_has_solution(const Puzzle* puzzle) {
    return puzzle->found;
}

int puzzle_get_solution_count(const Puzzle* puzzle) {
    return puzzle->solution_count;
}

void puzzle_set_cell(Puzzle* puzzle, int row, int col, int value) {
    if (row >= 0 && row < puzzle->n && col >= 0 && col < puzzle->n) {
        puzzle->board[row][col] = value;
    }
}

void puzzle_clear_cell(Puzzle* puzzle, int row, int col) {
    puzzle_set_cell(puzzle, row, col, -1);
}

void puzzle_save_solution(Puzzle* puzzle) {
    memcpy(puzzle->solution_board, puzzle->board, sizeof(puzzle->board));
    puzzle->found = true;
    puzzle->solution_count++;
}

void puzzle_mark_row_used(Puzzle* puzzle, int row, bool used) {
    if (row >= 0 && row < puzzle->n) {
        puzzle->row_used[row] = used;
    }
}

void puzzle_mark_col_used(Puzzle* puzzle, int col, bool used) {
    if (col >= 0 && col < puzzle->n) {
        puzzle->col_used[col] = used;
    }
}

bool puzzle_is_row_used(const Puzzle* puzzle, int row) {
    if (row >= 0 && row < puzzle->n) {
        return puzzle->row_used[row];
    }
    return false;
}

bool puzzle_is_col_used(const Puzzle* puzzle, int col) {
    if (col >= 0 && col < puzzle->n) {
        return puzzle->col_used[col];
    }
    return false;
}

void puzzle_build_region_lists(Puzzle* puzzle) {
    int n = puzzle->n;
    
    // Initialize region sizes to 0
    for (int i = 0; i < n; i++) {
        puzzle->region_size[i] = 0;
    }
    
    // Build lists of cells for each region
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            int region = puzzle->regions[row][col];
            if (region >= 0 && region < n) {
                int idx = puzzle->region_size[region];
                puzzle->region_cells_row[region][idx] = row;
                puzzle->region_cells_col[region][idx] = col;
                puzzle->region_size[region]++;
            }
        }
    }
}