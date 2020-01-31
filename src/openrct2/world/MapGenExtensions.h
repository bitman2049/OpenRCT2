/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include "MapGen.h"



void mapgen_generate_from_heightmap_2(mapgen_settings* settings);
bool mapgen_load_heightmap_2(const utf8* path);
static int32_t quarter_tile_smooth(int32_t x, int32_t y);
static Object* get_land_block_object(int texture, int type);
static void place_land_block(uint8_t x, uint8_t y, uint8_t z, uint8_t quadrant, uint8_t rotation, uint8_t slope, int texture);

#pragma region Landblocks

enum
{
    BLOCK_FLAT,
    BLOCK_1CORNER,
    BLOCK_SLOPE,
    BLOCK_3CORNER,
    BLOCK_DIAG,
};

struct NameRotation
{
    char name[9];
    int rotation;
};

// TERRAIN_GRASS
static constexpr const NameRotation GrassBlocks[] = {
    {"1KGRSBL0", 0},
    {"1KGRSBL3", 0},
    {"1KGRSBL1", 0},
    {"1KGRSBL2", 0},
    {"1KGRSB4F", 0},
};

// TERRAIN_SAND
static constexpr const char* SandBlocks[] = {
    "MGLBSF1 ", "MGLBSF5 ", "MGLBSF2 ", "MGLBSF3 ", "KNGBBD05",
};

// TERRAIN_DIRT
static constexpr const char* DirtBlocks[] = {
    "KNGBBD01", "KNGBBD04", "KNGBBD02", "KNGBBD03", "KNGBBD05",
};

// TERRAIN_ROCK
static constexpr const char* RockBlocks[] = {
    "XXBQRK21", "XXBQRK29", "XXBQRK22", "XXBQRK23", "KNGBBD05",
};

// TERRAIN_MARTIAN
static constexpr const char* MartianBlocks[] = {
    "1KLBRD1B", "1KLBRD4C", "1KLBRD3B", "1KLBRD2B", "KNGBBRC5",
};

// TERRAIN_GRASS_CLUMPS
static constexpr const char* GrassDirtBlocks[] = {
    "1KLBRD1B", "1KLBRD4C", "1KLBRD3B", "1KLBRD2B", "KNGBBRC5",
};

// TERRAIN_ICE
static constexpr const char* IceBlocks[] = {
    "LVBBICE1", "LVBBICE4", "LVBBICE5", "LVBBICE6", "LVBBICE8",
};

// TERRAIN_SAND_DARK
static constexpr const char* DarkSandBlocks[] = {
    "KNGBBRC1", "KNGBBRC4", "KNGBBRC2", "KNGBBRC3", "KNGBBRC5",
};

// TERRAIN_SAND_LIGHT
static constexpr const char* LightSandBlocks[] = {
    "XXBQRK11", "XXBQRK19", "XXBQRK12", "XXBQRK13", "KNGBBD05",
};

static Object* get_land_block_object(int texture, int type);

#pragma endregion

