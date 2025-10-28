#include "display.h"
#include <stdio.h>

DisplayOptions display_options_default(void) {
    DisplayOptions options;
    options.show_regions = false;
    options.use_colors = false;
    options.compact = false;
    return options;
}

void display_banner(void) {
    printf("╔════════════════════════════════════════╗\n");
    printf("║   LinkedIn Queens Puzzle Solver        ║\n");
    printf("║   OOP Design with Multiple Modules     ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
}

void display_help(void) {
    printf("Rules:\n");
    printf("  1. Board is divided into N regions\n");
    printf("  2. Each region must contain exactly 1 queen\n");
    printf("  3. No two queens in the same row\n");
    printf("  4. No two queens in the same column\n");
    printf("  5. No two queens diagonally adjacent (8-directional)\n\n");
    
    printf("Usage:\n");
    printf("  -a, --all           Find all solutions\n");
    printf("  -r, --regions       Show region map\n");
    printf("  -c, --continuous    Use continuous irregular regions (default, LinkedIn-like)\n");
    printf("  -s, --stripe        Use stripe regions (simple)\n");
    printf("  -i, --irregular     Use old irregular regions (not continuous)\n");
    printf("  -e, --edit          Interactive region editor\n");
    printf("  -h, --help          Show this help\n\n");
}

void display_regions(const Puzzle* puzzle) {
    int n = puzzle_get_size(puzzle);
    
    printf("\nRegion Map:\n");
    printf("┌");
    for (int i = 0; i < n; i++) printf("──");
    printf("─┐\n");
    
    for (int row = 0; row < n; row++) {
        printf("│ ");
        for (int col = 0; col < n; col++) {
            printf("%d ", puzzle_get_region(puzzle, row, col));
        }
        printf("│\n");
    }
    
    printf("└");
    for (int i = 0; i < n; i++) printf("──");
    printf("─┘\n");
}

void display_board(const Puzzle* puzzle, DisplayOptions options) {
    int n = puzzle_get_size(puzzle);
    
    if (options.show_regions) {
        display_regions(puzzle);
        printf("\n");
    }
    
    printf("Current Board:\n");
    printf("┌");
    for (int i = 0; i < n; i++) printf("──");
    printf("─┐\n");
    
    for (int row = 0; row < n; row++) {
        printf("│ ");
        for (int col = 0; col < n; col++) {
            int cell = puzzle_get_cell(puzzle, row, col);
            if (cell >= 0) {
                printf("Q ");
            } else if (options.show_regions) {
                printf("%d ", puzzle_get_region(puzzle, row, col));
            } else {
                printf(". ");
            }
        }
        printf("│\n");
    }
    
    printf("└");
    for (int i = 0; i < n; i++) printf("──");
    printf("─┘\n");
}

void display_solution(const Puzzle* puzzle, DisplayOptions options) {
    int n = puzzle_get_size(puzzle);
    
    if (!puzzle_has_solution(puzzle)) {
        printf("No solution found!\n");
        return;
    }
    
    if (options.show_regions) {
        display_regions(puzzle);
        printf("\n");
    }
    
    printf("Solution:\n");
    printf("┌");
    for (int i = 0; i < n; i++) printf("──");
    printf("─┐\n");
    
    for (int row = 0; row < n; row++) {
        printf("│ ");
        for (int col = 0; col < n; col++) {
            if (puzzle->solution_board[row][col] >= 0) {
                printf("Q ");
            } else if (options.show_regions) {
                printf("%d ", puzzle_get_region(puzzle, row, col));
            } else {
                printf(". ");
            }
        }
        printf("│\n");
    }
    
    printf("└");
    for (int i = 0; i < n; i++) printf("──");
    printf("─┘\n");
}

void display_stats(const Puzzle* puzzle) {
    printf("\nStatistics:\n");
    printf("  Board Size: %dx%d\n", puzzle_get_size(puzzle), puzzle_get_size(puzzle));
    printf("  Solutions Found: %d\n", puzzle_get_solution_count(puzzle));
    printf("  Status: %s\n", puzzle_has_solution(puzzle) ? "Solved ✓" : "No solution");
}