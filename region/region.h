/*
 * region.h - Region management module
 * Handles creation and manipulation of puzzle regions
 */

#ifndef REGION_H
#define REGION_H

#include "puzzle.h"

/* Region generation strategies */
typedef enum {
    REGION_STRIPE,      // Horizontal stripes
    REGION_IRREGULAR,   // Irregular/complex regions (LinkedIn-like)
    REGION_CUSTOM       // Load from custom data
} RegionType;

/* Create stripe regions (each row is a region) */
void region_create_stripe(Puzzle* puzzle);

/* Create irregular regions (more LinkedIn-like) */
void region_create_irregular(Puzzle* puzzle);

/* Load custom regions from 2D array */
void region_load_custom(Puzzle* puzzle, int regions[][MAX_N]);

/* Generate regions based on type */
void region_generate(Puzzle* puzzle, RegionType type);

/* Verify all regions are valid */
bool region_validate(const Puzzle* puzzle);

/* Count cells in a region */
int region_count_cells(const Puzzle* puzzle, int region_id);

#endif /* REGION_H */