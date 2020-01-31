/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../object/Object.h"
#include "Surface.h"

struct mapgen_settings
{
    // Base
    int32_t mapSize;
    int32_t height;
    int32_t water_level;
    int32_t floor;
    int32_t wall;

    // Features (e.g. tree, rivers, lakes etc.)
    int32_t trees;

    // Simplex Noise Parameters
    int32_t simplex_low;
    int32_t simplex_high;
    float simplex_base_freq;
    int32_t simplex_octaves;

    // Height map settings
    bool smooth;
    bool smooth_height_map;
    uint32_t smooth_strength;
    bool normalize_height;
};

void mapgen_generate_blank(mapgen_settings* settings);
void mapgen_generate(mapgen_settings* settings);
void mapgen_generate_custom_simplex(mapgen_settings* settings);
bool mapgen_load_heightmap(const utf8* path);
void mapgen_unload_heightmap();
void mapgen_generate_from_heightmap(mapgen_settings* settings);

#pragma region Height map struct

static struct
{
    uint32_t width, height;
    uint8_t* mono_bitmap;
} _heightMapData = { 0, 0, nullptr };

#pragma endregion Height map struct

#pragma region Random objects

static constexpr const char* GrassTrees[] = {
    // Dark
    "TCF     ", // Caucasian Fir Tree
    "TRF     ", // Red Fir Tree
    "TRF2    ", // Red Fir Tree
    "TSP     ", // Scots Pine Tree
    "TMZP    ", // Montezuma Pine Tree
    "TAP     ", // Aleppo Pine Tree
    "TCRP    ", // Corsican Pine Tree
    "TBP     ", // Black Poplar Tree

    // Light
    "TCL     ", // Cedar of Lebanon Tree
    "TEL     ", // European Larch Tree
};

static constexpr const char* DesertTrees[] = {
    "TMP     ", // Monkey-Puzzle Tree
    "THL     ", // Honey Locust Tree
    "TH1     ", // Canary Palm Tree
    "TH2     ", // Palm Tree
    "TPM     ", // Palm Tree
    "TROPT1  ", // Tree
    "TBC     ", // Cactus
    "TSC     ", // Cactus
};

static constexpr const char* SnowTrees[] = {
    "TCFS    ", // Snow-covered Caucasian Fir Tree
    "TNSS    ", // Snow-covered Norway Spruce Tree
    "TRF3    ", // Snow-covered Red Fir Tree
    "TRFS    ", // Snow-covered Red Fir Tree
};

#pragma endregion

// Randomly chosen base terrains. We rarely want a whole map made out of chequerboard or rock.
static constexpr const uint8_t BaseTerrain[] = { TERRAIN_GRASS, TERRAIN_SAND, TERRAIN_SAND_LIGHT, TERRAIN_DIRT, TERRAIN_ICE };

static void mapgen_place_trees();
static void mapgen_set_water_level(int32_t waterLevel);
static void mapgen_smooth_height(int32_t iterations);
static void mapgen_set_height();
static void mapgen_smooth_heightmap(uint8_t* src, int32_t strength);
static void mapgen_simplex(mapgen_settings* settings);

static int32_t _heightSize;
static uint8_t* _height;
