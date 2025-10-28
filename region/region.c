#include "region.h"
#include <string.h>

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