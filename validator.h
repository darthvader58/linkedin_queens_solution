/*
 * validator.h - Solution validation module
 * Checks if puzzle solutions satisfy all constraints
 */

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "puzzle.h"

/* Check if two positions are diagonally adjacent */
bool validator_is_diagonally_adjacent(const Puzzle* puzzle, int row1, int col1, int row2, int col2);

/* Check if any queen is diagonally adjacent to position */
bool validator_has_adjacent_queen(const Puzzle* puzzle, int row, int col);

/* Check if placing a queen at (row, col) is valid */
bool validator_can_place(const Puzzle* puzzle, int row, int col, int region);

/* Verify the complete solution satisfies all constraints */
bool validator_verify_solution(const Puzzle* puzzle);

/* Check specific constraint violations */
bool validator_check_rows(const Puzzle* puzzle);
bool validator_check_columns(const Puzzle* puzzle);
bool validator_check_regions(const Puzzle* puzzle);
bool validator_check_diagonals(const Puzzle* puzzle);

#endif /* VALIDATOR_H */