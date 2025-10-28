/*
 * solver.h - Puzzle solving algorithms
 * Contains backtracking solver implementation
 */

#ifndef SOLVER_H
#define SOLVER_H

#include "puzzle.h"

/* Solver configuration */
typedef struct {
    bool find_all_solutions;    // If true, find all solutions
    bool verbose;               // Print progress
    double time_limit;          // Maximum time in seconds (0 = no limit)
} SolverConfig;

/* Create default solver configuration */
SolverConfig solver_config_default(void);

/* Main solving function */
bool solver_solve(Puzzle* puzzle, SolverConfig config);

/* Backtracking helper (internal) */
void solver_backtrack(Puzzle* puzzle, int region, SolverConfig config);

/* Get solving statistics */
typedef struct {
    double solve_time;
    int solutions_found;
    int nodes_explored;
} SolverStats;

SolverStats solver_get_stats(const Puzzle* puzzle);

#endif /* SOLVER_H */