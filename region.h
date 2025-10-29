#ifndef REGION_H
#define REGION_H

#include "puzzle.h"

/* Region generation strategies */
typedef enum {
    REGION_CONTINUOUS,  // Continuous irregular regions (LINKEDIN-LIKE)
    REGION_CUSTOM,      // Load from custom data
    REGION_INTERACTIVE  // Interactive creation
} RegionType;

/* Load custom regions from 2D array */
void region_load_custom(Puzzle* puzzle, int regions[][MAX_N]);

/* Generate regions based on type */
void region_generate(Puzzle* puzzle, RegionType type);

/* Verify all regions are valid */
bool region_validate(const Puzzle* puzzle);

/* Count cells in a region */
int region_count_cells(const Puzzle* puzzle, int region_id);

/* Generate continuous irregular regions (LINKEDIN-LIKE) - Works for any N×N board */
void region_generate_continuous(Puzzle* puzzle);

/* Verify that all regions are continuous/connected */
bool region_verify_continuous(const Puzzle* puzzle);

/* Interactive region creator */
void region_create_interactive(Puzzle* puzzle);

#endif