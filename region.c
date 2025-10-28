#include "region.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void region_create_stripe(Puzzle* puzzle) {
    int n = puzzle_get_size(puzzle);
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            puzzle->regions[row][col] = row;
        }
    }
}

void region_create_irregular(Puzzle* puzzle) {
    int n = puzzle_get_size(puzzle);
    
    // Example 8x8 irregular pattern (LinkedIn-like)
    if (n == 8) {
        int pattern[8][8] = {
            {0, 0, 1, 1, 2, 2, 3, 3},
            {0, 0, 1, 1, 2, 2, 3, 3},
            {4, 4, 5, 5, 6, 6, 7, 7},
            {4, 4, 5, 5, 6, 6, 7, 7},
            {0, 1, 1, 2, 2, 3, 3, 7},
            {0, 4, 5, 5, 6, 6, 7, 7},
            {4, 4, 5, 5, 6, 6, 1, 2},
            {0, 4, 5, 3, 3, 2, 2, 7}
        };
        
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                puzzle->regions[row][col] = pattern[row][col];
            }
        }
    } else {
        // Default to stripe for other sizes
        region_create_stripe(puzzle);
    }
}

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
        case REGION_STRIPE:
            region_create_stripe(puzzle);
            break;
        case REGION_IRREGULAR:
            region_create_irregular(puzzle);
            break;
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
 * Count unassigned neighbors
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
 * Get all neighbors of a given region
 */
static void get_region_neighbors(int regions[][MAX_N], int n, int region_id, 
                                 int* neighbors_r, int* neighbors_c, int* count) {
    *count = 0;
    
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
                        for (int i = 0; i < *count; i++) {
                            if (neighbors_r[i] == nr && neighbors_c[i] == nc) {
                                already_added = true;
                                break;
                            }
                        }
                        
                        if (!already_added) {
                            neighbors_r[*count] = nr;
                            neighbors_c[*count] = nc;
                            (*count)++;
                        }
                    }
                }
            }
        }
    }
}

/*
 * Generate continuous irregular regions using region-growing algorithm
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
    srand((unsigned int)time(NULL));
    
    // Arrays to store potential neighbors
    int neighbors_r[MAX_N * MAX_N];
    int neighbors_c[MAX_N * MAX_N];
    
    // Generate n regions
    int total_assigned = 0;
    int target_size = n;  // Each region should have approximately n cells
    
    for (int region_id = 0; region_id < n; region_id++) {
        int region_size = 0;
        int start_row, start_col;
        
        // Pick starting cell
        if (region_id == 0) {
            // First region - pick random cell
            start_row = rand() % n;
            start_col = rand() % n;
        } else {
            // Find a cell adjacent to existing regions
            bool found = false;
            for (int attempt = 0; attempt < 100 && !found; attempt++) {
                int check_region = rand() % region_id;
                int neighbor_count;
                get_region_neighbors(regions, n, check_region, neighbors_r, neighbors_c, &neighbor_count);
                
                if (neighbor_count > 0) {
                    int idx = rand() % neighbor_count;
                    start_row = neighbors_r[idx];
                    start_col = neighbors_c[idx];
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
        total_assigned++;
        
        // Grow region
        while (region_size < target_size && total_assigned < n * n) {
            int neighbor_count;
            get_region_neighbors(regions, n, region_id, neighbors_r, neighbors_c, &neighbor_count);
            
            if (neighbor_count == 0) break;
            
            // Pick a random neighbor
            int idx = rand() % neighbor_count;
            int next_row = neighbors_r[idx];
            int next_col = neighbors_c[idx];
            
            regions[next_row][next_col] = region_id;
            region_size++;
            total_assigned++;
        }
    }
    
    // Assign remaining cells to nearest region
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (regions[row][col] == -1) {
                // Find nearest assigned neighbor
                for (int d = 0; d < 4; d++) {
                    int nr = row + DX4[d];
                    int nc = col + DY4[d];
                    
                    if (nr >= 0 && nr < n && nc >= 0 && nc < n && regions[nr][nc] != -1) {
                        regions[row][col] = regions[nr][nc];
                        break;
                    }
                }
                
                if (regions[row][col] == -1) {
                    regions[row][col] = 0;
                }
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