#include "region.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void region_load_custom(Puzzle* puzzle, int regions[][MAX_N]) {
    int n = puzzle_get_size(puzzle);
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            puzzle->regions[row][col] = regions[row][col];
        }
    }
}

void region_generate(Puzzle* puzzle, RegionType type) {
    switch (type) {
        case REGION_CONTINUOUS:
            region_generate_continuous(puzzle);
            break;
        case REGION_INTERACTIVE:
            region_create_interactive(puzzle);
            break;
        case REGION_CUSTOM:
            // Custom regions should be loaded separately
            break;
    }
}

bool region_validate(const Puzzle* puzzle) {
    int n = puzzle_get_size(puzzle);
    int region_count[MAX_N] = {0};
    int max_region = -1;
    
    // Count cells in each region
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            int region = puzzle_get_region(puzzle, row, col);
            if (region < 0 || region >= MAX_N) {
                return false;
            }
            region_count[region]++;
            if (region > max_region) {
                max_region = region;
            }
        }
    }
    
    // Check that we have exactly n regions
    if (max_region != n - 1) {
        return false;
    }
    
    // Each region should have at least one cell
    for (int i = 0; i < n; i++) {
        if (region_count[i] == 0) {
            return false;
        }
    }
    
    return true;
}

int region_count_cells(const Puzzle* puzzle, int region_id) {
    int n = puzzle_get_size(puzzle);
    int count = 0;
    
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (puzzle_get_region(puzzle, row, col) == region_id) {
                count++;
            }
        }
    }
    
    return count;
}

// 4-directional neighbors (for connectivity)
static const int DX4[] = {-1, 0, 1, 0};
static const int DY4[] = {0, 1, 0, -1};

/*
 * Check if a cell is valid and unassigned
 */
static bool is_valid_unassigned(int regions[][MAX_N], int n, int row, int col) {
    return row >= 0 && row < n && col >= 0 && col < n && regions[row][col] == -1;
}

/*
 * Count unassigned neighbors (4-directional)
 */
static int count_unassigned_neighbors(int regions[][MAX_N], int n, int row, int col) {
    int count = 0;
    for (int d = 0; d < 4; d++) {
        int nr = row + DX4[d];
        int nc = col + DY4[d];
        if (is_valid_unassigned(regions, n, nr, nc)) {
            count++;
        }
    }
    return count;
}

/*
 * Get all frontier cells (unassigned cells adjacent to a specific region)
 */
static int get_region_frontier(int regions[][MAX_N], int n, int region_id, 
                                int* frontier_r, int* frontier_c) {
    int count = 0;
    
    // Find all cells in this region
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (regions[row][col] == region_id) {
                // Check neighbors of this cell
                for (int d = 0; d < 4; d++) {
                    int nr = row + DX4[d];
                    int nc = col + DY4[d];
                    
                    if (is_valid_unassigned(regions, n, nr, nc)) {
                        // Check if not already in list
                        bool already_added = false;
                        for (int i = 0; i < count; i++) {
                            if (frontier_r[i] == nr && frontier_c[i] == nc) {
                                already_added = true;
                                break;
                            }
                        }
                        
                        if (!already_added) {
                            frontier_r[count] = nr;
                            frontier_c[count] = nc;
                            count++;
                        }
                    }
                }
            }
        }
    }
    
    return count;
}

/*
 * Find the best next cell to add to a region
 * Prefer cells with fewer unassigned neighbors to avoid creating isolated cells
 */
static bool find_best_frontier_cell(int regions[][MAX_N], int n, int region_id,
                                     int* best_row, int* best_col) {
    int frontier_r[MAX_N * MAX_N];
    int frontier_c[MAX_N * MAX_N];
    int frontier_count = get_region_frontier(regions, n, region_id, frontier_r, frontier_c);
    
    if (frontier_count == 0) {
        return false;
    }
    
    // Find the frontier cell with the fewest unassigned neighbors
    int min_neighbors = 5;
    int best_idx = -1;
    
    for (int i = 0; i < frontier_count; i++) {
        int neighbors = count_unassigned_neighbors(regions, n, frontier_r[i], frontier_c[i]);
        if (neighbors < min_neighbors || (neighbors == min_neighbors && rand() % 2 == 0)) {
            min_neighbors = neighbors;
            best_idx = i;
        }
    }
    
    if (best_idx >= 0) {
        *best_row = frontier_r[best_idx];
        *best_col = frontier_c[best_idx];
        return true;
    }
    
    return false;
}

/*
 * Generate continuous irregular regions using improved region-growing algorithm
 * Works for any N×N board
 */
void region_generate_continuous(Puzzle* puzzle) {
    int n = puzzle_get_size(puzzle);
    
    // Use puzzle's internal regions array
    int regions[MAX_N][MAX_N];
    
    // Initialize to -1 (unassigned)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            regions[i][j] = -1;
        }
    }
    
    // Seed random number generator with current time
    static int seed_initialized = 0;
    if (!seed_initialized) {
        srand((unsigned int)time(NULL));
        seed_initialized = 1;
    }
    
    // Calculate target size for each region (with some variation)
    int total_cells = n * n;
    int cells_assigned = 0;
    
    // Generate n regions
    for (int region_id = 0; region_id < n; region_id++) {
        int region_size = 0;
        int start_row, start_col;
        
        // Determine target size for this region
        int remaining_cells = total_cells - cells_assigned;
        int remaining_regions = n - region_id;
        int target_size = remaining_cells / remaining_regions;
        
        // Add some randomness (±20%)
        int variation = target_size / 5;
        if (variation > 0 && region_id < n - 1) {
            target_size += (rand() % (2 * variation + 1)) - variation;
        }
        
        // Ensure at least 1 cell per region
        if (target_size < 1) target_size = 1;
        
        // Pick starting cell
        if (region_id == 0) {
            // First region - pick random cell
            start_row = rand() % n;
            start_col = rand() % n;
        } else {
            // Find an unassigned cell adjacent to any existing region
            bool found = false;
            
            // Try multiple attempts to find a good starting cell
            for (int attempt = 0; attempt < 100 && !found; attempt++) {
                // Pick a random existing region
                int check_region = rand() % region_id;
                int frontier_r[MAX_N * MAX_N];
                int frontier_c[MAX_N * MAX_N];
                int frontier_count = get_region_frontier(regions, n, check_region, 
                                                        frontier_r, frontier_c);
                
                if (frontier_count > 0) {
                    int idx = rand() % frontier_count;
                    start_row = frontier_r[idx];
                    start_col = frontier_c[idx];
                    found = true;
                }
            }
            
            // Fallback: find any unassigned cell
            if (!found) {
                for (int row = 0; row < n && !found; row++) {
                    for (int col = 0; col < n && !found; col++) {
                        if (regions[row][col] == -1) {
                            start_row = row;
                            start_col = col;
                            found = true;
                        }
                    }
                }
            }
        }
        
        // Assign starting cell
        regions[start_row][start_col] = region_id;
        region_size++;
        cells_assigned++;
        
        // Grow region using best-first strategy
        while (region_size < target_size && cells_assigned < total_cells) {
            int next_row, next_col;
            
            if (find_best_frontier_cell(regions, n, region_id, &next_row, &next_col)) {
                regions[next_row][next_col] = region_id;
                region_size++;
                cells_assigned++;
            } else {
                // No more cells can be added to this region
                break;
            }
        }
    }
    
    // Assign remaining cells to nearest region (should be rare or none)
    for (int pass = 0; pass < n && cells_assigned < total_cells; pass++) {
        for (int row = 0; row < n; row++) {
            for (int col = 0; col < n; col++) {
                if (regions[row][col] == -1) {
                    // Find nearest assigned neighbor
                    for (int d = 0; d < 4; d++) {
                        int nr = row + DX4[d];
                        int nc = col + DY4[d];
                        
                        if (nr >= 0 && nr < n && nc >= 0 && nc < n && regions[nr][nc] != -1) {
                            regions[row][col] = regions[nr][nc];
                            cells_assigned++;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    // Final fallback: assign any remaining cells to region 0
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (regions[row][col] == -1) {
                regions[row][col] = 0;
                cells_assigned++;
            }
        }
    }
    
    // Copy to puzzle
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            puzzle->regions[row][col] = regions[row][col];
        }
    }
}

/*
 * Verify that all regions are continuous/connected
 */
bool region_verify_continuous(const Puzzle* puzzle) {
    int n = puzzle_get_size(puzzle);
    
    for (int region_id = 0; region_id < n; region_id++) {
        // Find first cell of this region
        int start_row = -1, start_col = -1;
        int total_cells = 0;
        
        for (int row = 0; row < n && start_row == -1; row++) {
            for (int col = 0; col < n && start_col == -1; col++) {
                if (puzzle_get_region(puzzle, row, col) == region_id) {
                    start_row = row;
                    start_col = col;
                }
            }
        }
        
        if (start_row == -1) continue;
        
        // Count total cells in this region
        for (int row = 0; row < n; row++) {
            for (int col = 0; col < n; col++) {
                if (puzzle_get_region(puzzle, row, col) == region_id) {
                    total_cells++;
                }
            }
        }
        
        // DFS to check connectivity
        bool visited[MAX_N][MAX_N] = {{false}};
        int stack_r[MAX_N * MAX_N];
        int stack_c[MAX_N * MAX_N];
        int stack_top = 0;
        
        stack_r[stack_top] = start_row;
        stack_c[stack_top] = start_col;
        stack_top++;
        visited[start_row][start_col] = true;
        
        int connected_count = 1;
        
        while (stack_top > 0) {
            stack_top--;
            int row = stack_r[stack_top];
            int col = stack_c[stack_top];
            
            for (int d = 0; d < 4; d++) {
                int nr = row + DX4[d];
                int nc = col + DY4[d];
                
                if (nr >= 0 && nr < n && nc >= 0 && nc < n && 
                    !visited[nr][nc] && 
                    puzzle_get_region(puzzle, nr, nc) == region_id) {
                    
                    visited[nr][nc] = true;
                    stack_r[stack_top] = nr;
                    stack_c[stack_top] = nc;
                    stack_top++;
                    connected_count++;
                }
            }
        }
        
        if (connected_count != total_cells) {
            return false;
        }
    }
    
    return true;
}

/*
 * Interactive region creator
 */
void region_create_interactive(Puzzle* puzzle) {
    int n = puzzle_get_size(puzzle);
    
    // Initialize all to -1 (unassigned)
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            puzzle->regions[row][col] = -1;
        }
    }
    
    printf("\n=== Interactive Region Creator ===\n");
    printf("Create %d regions by assigning cells.\n", n);
    printf("Commands:\n");
    printf("  <row> <col> <region>  - Assign cell to region (0-%d)\n", n-1);
    printf("  show                  - Show current regions\n");
    printf("  auto                  - Auto-fill remaining cells\n");
    printf("  done                  - Finish\n\n");
    
    int cells_assigned = 0;
    char cmd[100];
    
    while (cells_assigned < n * n) {
        // Show current state
        printf("\nCurrent regions (%d/%d cells assigned):\n", cells_assigned, n * n);
        for (int row = 0; row < n; row++) {
            for (int col = 0; col < n; col++) {
                int r = puzzle_get_region(puzzle, row, col);
                if (r == -1) {
                    printf(". ");
                } else {
                    printf("%d ", r);
                }
            }
            printf("\n");
        }
        
        printf("\nCommand: ");
        if (!fgets(cmd, sizeof(cmd), stdin)) break;
        
        if (strncmp(cmd, "done", 4) == 0) {
            break;
        } else if (strncmp(cmd, "show", 4) == 0) {
            continue;
        } else if (strncmp(cmd, "auto", 4) == 0) {
            // Auto-fill
            for (int row = 0; row < n; row++) {
                for (int col = 0; col < n; col++) {
                    if (puzzle->regions[row][col] == -1) {
                        for (int d = 0; d < 4; d++) {
                            int nr = row + DX4[d];
                            int nc = col + DY4[d];
                            if (nr >= 0 && nr < n && nc >= 0 && nc < n &&
                                puzzle->regions[nr][nc] != -1) {
                                puzzle->regions[row][col] = puzzle->regions[nr][nc];
                                cells_assigned++;
                                break;
                            }
                        }
                    }
                }
            }
        } else {
            int row, col, region;
            if (sscanf(cmd, "%d %d %d", &row, &col, &region) == 3) {
                if (row >= 0 && row < n && col >= 0 && col < n && 
                    region >= 0 && region < n) {
                    if (puzzle->regions[row][col] == -1) {
                        puzzle->regions[row][col] = region;
                        cells_assigned++;
                    } else {
                        printf("Cell already assigned!\n");
                    }
                } else {
                    printf("Invalid! Must be 0-%d\n", n-1);
                }
            }
        }
    }
}