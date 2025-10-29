#include "solver.h"
#include "validator.h"
#include <stdio.h>
#include <time.h>

static double g_solve_time = 0.0;
static int g_nodes_explored = 0;

SolverConfig solver_config_default(void) {
    SolverConfig config;
    config.find_all_solutions = false;
    config.verbose = false;
    config.time_limit = 0.0;
    return config;
}

void solver_backtrack(Puzzle* puzzle, int region, SolverConfig config) {
    int n = puzzle_get_size(puzzle);
    
    if (region == n) {
        // Found a complete solution
        if (puzzle->solution_count == 0) {
            puzzle_save_solution(puzzle);
        } else {
            puzzle->solution_count++;
        }
        return;
    }
    
    // Early termination if we found a solution and don't need all
    if (!config.find_all_solutions && puzzle_has_solution(puzzle)) {
        return;
    }
    
    // OPTIMIZATION: Only try cells that belong to this region
    int region_cell_count = puzzle->region_size[region];
    
    for (int i = 0; i < region_cell_count; i++) {
        int row = puzzle->region_cells_row[region][i];
        int col = puzzle->region_cells_col[region][i];
        
        g_nodes_explored++;
        
        if (validator_can_place(puzzle, row, col, region)) {
            // Place queen
            puzzle_set_cell(puzzle, row, col, region);
            puzzle_mark_row_used(puzzle, row, true);
            puzzle_mark_col_used(puzzle, col, true);
            
            // Recurse to next region
            solver_backtrack(puzzle, region + 1, config);
            
            // Backtrack
            puzzle_clear_cell(puzzle, row, col);
            puzzle_mark_row_used(puzzle, row, false);
            puzzle_mark_col_used(puzzle, col, false);
            
            // Early exit if found and don't need all
            if (!config.find_all_solutions && puzzle_has_solution(puzzle)) {
                return;
            }
        }
    }
}

bool solver_solve(Puzzle* puzzle, SolverConfig config) {
    clock_t start = clock();
    g_nodes_explored = 0;
    
    solver_backtrack(puzzle, 0, config);
    
    clock_t end = clock();
    g_solve_time = (double)(end - start) / CLOCKS_PER_SEC;
    
    if (config.verbose) {
        printf("Solved %dx%d board in %.6f seconds\n", 
               puzzle_get_size(puzzle), puzzle_get_size(puzzle), g_solve_time);
        printf("Explored %d nodes\n", g_nodes_explored);
        printf("Found %d solution(s)\n", puzzle_get_solution_count(puzzle));
    }
    
    return puzzle_has_solution(puzzle);
}

SolverStats solver_get_stats(const Puzzle* puzzle) {
    SolverStats stats;
    stats.solve_time = g_solve_time;
    stats.solutions_found = puzzle_get_solution_count(puzzle);
    stats.nodes_explored = g_nodes_explored;
    return stats;
}