#include "puzzle.h"
#include "region.h"
#include "solver.h"
#include "validator.h"
#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int board_size;
    bool find_all;
    bool show_regions;
    bool show_help;
    RegionType region_type;
    bool verbose;
} AppConfig;

AppConfig parse_arguments(int argc, char* argv[]) {
    AppConfig config;
    config.board_size = 8;  // Default
    config.find_all = false;
    config.show_regions = false;
    config.show_help = false;
    config.region_type = REGION_CONTINUOUS;  // LinkedIn-like continuous regions
    config.verbose = true;
    
    if (argc < 2) {
        config.show_help = true;
        return config;
    }
    
    config.board_size = atoi(argv[1]);
    
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--all") == 0) {
            config.find_all = true;
        } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--regions") == 0) {
            config.show_regions = true;
        } else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--edit") == 0) {
            config.region_type = REGION_INTERACTIVE;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            config.show_help = true;
        } else if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quiet") == 0) {
            config.verbose = false;
        }
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    // Parse command-line arguments
    AppConfig config = parse_arguments(argc, argv);
    
    // Display banner
    display_banner();
    
    // Show help if requested or invalid args
    if (config.show_help || config.board_size < 1 || config.board_size > MAX_N) {
        display_help();
        printf("Example: %s 8 --regions --all\n", argv[0]);
        return config.board_size < 1 || config.board_size > MAX_N ? 1 : 0;
    }
    
    // Create puzzle instance
    Puzzle* puzzle = puzzle_create(config.board_size);
    if (!puzzle) {
        fprintf(stderr, "Error: Failed to create puzzle\n");
        return 1;
    }
    
    // Generate regions
    region_generate(puzzle, config.region_type);
    
    // Validate regions
    if (!region_validate(puzzle)) {
        fprintf(stderr, "Error: Invalid region configuration\n");
        puzzle_destroy(puzzle);
        return 1;
    }
    
    // Check if regions are continuous
    if (config.verbose) {
        if (region_verify_continuous(puzzle)) {
            printf("✓ All regions are continuous\n\n");
        } else {
            printf("⚠ Warning: Some regions are not continuous\n\n");
        }
    }
    
    // Configure solver
    SolverConfig solver_config = solver_config_default();
    solver_config.find_all_solutions = config.find_all;
    solver_config.verbose = config.verbose;
    
    // Configure display
    DisplayOptions display_options = display_options_default();
    display_options.show_regions = config.show_regions;
    
    // Solve the puzzle
    printf("Solving %dx%d puzzle...\n\n", config.board_size, config.board_size);
    
    if (solver_solve(puzzle, solver_config)) {
        // Display solution
        display_solution(puzzle, display_options);
        
        // Validate solution
        printf("\nValidating solution...\n");
        if (validator_verify_solution(puzzle)) {
            printf("✓ Solution verified successfully!\n");
        } else {
            printf("✗ Solution validation failed!\n");
        }
        
        // Display statistics
        if (config.verbose) {
            display_stats(puzzle);
        }
    } else {
        printf("No solution exists for this configuration!\n");
    }
    
    // Cleanup
    puzzle_destroy(puzzle);
    
    return 0;
}