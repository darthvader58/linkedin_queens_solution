#ifndef PUZZLE_H
#define PUZZLE_H

#include <stdbool.h>

#define MAX_N 16

typedef struct {
    int n;                              // Board size
    int board[MAX_N][MAX_N];            // Current board state (-1 = empty, 0+ = region id)
    int solution_board[MAX_N][MAX_N];   // Saved solution
    int regions[MAX_N][MAX_N];          // Region ID for each cell
    bool row_used[MAX_N];               // Track used rows
    bool col_used[MAX_N];               // Track used columns
    int solution_count;                 // Number of solutions found
    bool found;                         // Flag for first solution found
} Puzzle;

/* Constructor-like function */
Puzzle* puzzle_create(int n);

/* Destructor-like function */
void puzzle_destroy(Puzzle* puzzle);

/* Initialize/reset puzzle state */
void puzzle_init(Puzzle* puzzle, int n);

/* Getter methods */
int puzzle_get_size(const Puzzle* puzzle);
int puzzle_get_cell(const Puzzle* puzzle, int row, int col);
int puzzle_get_region(const Puzzle* puzzle, int row, int col);
bool puzzle_has_solution(const Puzzle* puzzle);
int puzzle_get_solution_count(const Puzzle* puzzle);

/* Setter methods */
void puzzle_set_cell(Puzzle* puzzle, int row, int col, int value);
void puzzle_clear_cell(Puzzle* puzzle, int row, int col);

/* State management */
void puzzle_save_solution(Puzzle* puzzle);
void puzzle_mark_row_used(Puzzle* puzzle, int row, bool used);
void puzzle_mark_col_used(Puzzle* puzzle, int col, bool used);
bool puzzle_is_row_used(const Puzzle* puzzle, int row);
bool puzzle_is_col_used(const Puzzle* puzzle, int col);

#endif