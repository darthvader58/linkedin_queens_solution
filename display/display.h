/*
 * display.h - Display and visualization module
 * Handles all terminal output and board rendering
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "puzzle.h"

/* Display options */
typedef struct {
    bool show_regions;      // Show region numbers
    bool use_colors;        // Use ANSI colors
    bool compact;           // Compact display mode
} DisplayOptions;

/* Create default display options */
DisplayOptions display_options_default(void);

/* Display the puzzle board */
void display_board(const Puzzle* puzzle, DisplayOptions options);

/* Display the solution board */
void display_solution(const Puzzle* puzzle, DisplayOptions options);

/* Display region map */
void display_regions(const Puzzle* puzzle);

/* Display game header/banner */
void display_banner(void);

/* Display help/instructions */
void display_help(void);

/* Display statistics */
void display_stats(const Puzzle* puzzle);

#endif /* DISPLAY_H */