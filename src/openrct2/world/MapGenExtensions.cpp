/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapGenExtensions.h"

#include "../Context.h"
#include "../Game.h"
#include "../common.h"
#include "../core/Guard.hpp"
#include "../core/Imaging.h"
#include "../core/String.hpp"
#include "../localisation/StringIds.h"
#include "../object/Object.h"
#include "../object/ObjectManager.h"
#include "../platform/platform.h"
#include "../util/Util.h"
#include "Map.h"
#include "MapGen.h"
#include "MapHelpers.h"
#include "Scenery.h"
#include "SmallScenery.h"
#include "Surface.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iterator>
#include <vector>

void mapgen_generate_from_heightmap_2(mapgen_settings* settings)
{
    openrct2_assert(_heightMapData.width == _heightMapData.height, "Invalid height map size");
    openrct2_assert(_heightMapData.mono_bitmap != nullptr, "No height map loaded");
    openrct2_assert(settings->simplex_high != settings->simplex_low, "Low and high setting cannot be the same");

    // Make a copy of the original height map that we can edit
    uint8_t* dest = new uint8_t[_heightMapData.width * _heightMapData.height];
    std::memcpy(dest, _heightMapData.mono_bitmap, _heightMapData.width * _heightMapData.width);

    map_init((_heightMapData.width) / 2 + 2); // + 2 for the black tiles around the map

    if (settings->smooth_height_map)
    {
        //mapgen_smooth_heightmap(dest, settings->smooth_strength);
        int i = 0;
        if (i)
        {
            quarter_tile_smooth(0, 0);
            get_land_block_object(TERRAIN_GRASS, BLOCK_FLAT);
            place_land_block(0, 0, 0, 0, 0, 0, 0);
        }
    }

    uint8_t maxValue = 255;
    uint8_t minValue = 0;

    if (settings->normalize_height)
    {
        // Get highest and lowest pixel value
        maxValue = 0;
        minValue = 0xff;
        for (uint32_t y = 0; y < _heightMapData.height; y++)
        {
            for (uint32_t x = 0; x < _heightMapData.width; x++)
            {
                uint8_t value = dest[x + y * _heightMapData.width];
                maxValue = std::max(maxValue, value);
                minValue = std::min(minValue, value);
            }
        }

        if (minValue == maxValue)
        {
            context_show_error(STR_HEIGHT_MAP_ERROR, STR_ERROR_CANNOT_NORMALIZE);
            delete[] dest;
            return;
        }
    }

    openrct2_assert(maxValue > minValue, "Input range is invalid");
    openrct2_assert(settings->simplex_high > settings->simplex_low, "Output range is invalid");

    const uint8_t rangeIn = maxValue - minValue;
    const uint8_t rangeOut = settings->simplex_high - settings->simplex_low;

    for (uint32_t y = 0; y < _heightMapData.height; y++)
    {
        for (uint32_t x = 0; x < _heightMapData.width; x++)
        {
            // The x and y axis are flipped in the world, so this uses y for x and x for y.
            auto* const surfaceElement = map_get_surface_element_at(
                TileCoordsXY{ static_cast<int32_t>(y / 2 + 1), static_cast<int32_t>(x / 2 + 1) }.ToCoordsXY());
            if (surfaceElement == nullptr)
                continue;

            // Read value from bitmap, and convert its range
            uint8_t value = dest[x + y * _heightMapData.width];
            value = (uint8_t)((float)(value - minValue) / rangeIn * rangeOut) + settings->simplex_low;
            if ((x % 2 == 0) && (y % 2 == 0))
                surfaceElement->base_height = value;
            else
                surfaceElement->base_height = std::min(surfaceElement->base_height, value);

            // Floor to even number
            surfaceElement->base_height /= 2;
            surfaceElement->base_height *= 2;
            surfaceElement->clearance_height = surfaceElement->base_height;

            // Set water level
            if (surfaceElement->base_height < settings->water_level)
            {
                surfaceElement->SetWaterHeight(settings->water_level * COORDS_Z_STEP);
            }
        }
    }

    // Clean up
    delete[] dest;
}

static int32_t quarter_tile_smooth(int32_t x, int32_t y)
{
    return 0;
}

static Object* get_land_block_object(int texture, int type)
{
    rct_object_entry entry;
    entry.flags = 0x00008000 + OBJECT_TYPE_SMALL_SCENERY;
    entry.checksum = 0;
    switch (texture)
    {
        case TERRAIN_GRASS:
            std::copy_n(GrassBlocks[type].name, 8, entry.name);
            break;
        case TERRAIN_SAND:
            std::copy_n(SandBlocks[type], 8, entry.name);
            break;
        case TERRAIN_DIRT:
            std::copy_n(DirtBlocks[type], 8, entry.name);
            break;
        case TERRAIN_ROCK:
            std::copy_n(RockBlocks[type], 8, entry.name);
            break;
        case TERRAIN_MARTIAN:
            std::copy_n(MartianBlocks[type], 8, entry.name);
            break;
        case TERRAIN_GRASS_CLUMPS:
            std::copy_n(GrassDirtBlocks[type], 8, entry.name);
            break;
        case TERRAIN_ICE:
            std::copy_n(IceBlocks[type], 8, entry.name);
            break;
        case TERRAIN_SAND_DARK:
            std::copy_n(DarkSandBlocks[type], 8, entry.name);
            break;
        case TERRAIN_SAND_LIGHT:
            std::copy_n(LightSandBlocks[type], 8, entry.name);
            break;
        default:
            return nullptr;
    }

    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    return objectManager.LoadObject(&entry);
}

static void place_land_block(uint8_t x, uint8_t y, uint8_t z, uint8_t quadrant, uint8_t rotation, uint8_t slope, int texture)
{
    int blockType = 0;
    switch (slope & 0xF)
    {
        // BLOCK_FLAT
        case 0b1111:
            //  +---+
            //  |   |
            //  +---+
            z++;
        case 0b0000:
            //  o---o
            //  |   |
            //  o---o
            blockType = BLOCK_FLAT;
            break;

        // BLOCK_1CORNER
        case 0b0001:
            //  +---o
            //  | / |
            //  o---o
            rotation++;
        case 0b0010:
            //  o---+
            //  | \ |
            //  o---o
            rotation++;
        case 0b0100:
            //  o---o
            //  | / |
            //  o---+
            rotation++;
        case 0b1000:
            //  o---o
            //  | \ |
            //  +---o
            blockType = BLOCK_1CORNER;
            break;

        // BLOCK_SLOPE
        case 0b0011:
            //  +---+
            //  |---|
            //  o---o
            rotation++;
        case 0b0110:
            //  o---+
            //  | | |
            //  o---+
            rotation++;
        case 0b1100:
            //  o---o
            //  |---|
            //  +---+
            rotation++;
        case 0b1001:
            //  +---o
            //  | | |
            //  +---o
            blockType = BLOCK_SLOPE;
            break;

        // BLOCK_3CORNER
        case 0b0111:
            //  +---+
            //  | \ |
            //  o---+
            rotation++;
        case 0b1110:
            //  o---+
            //  | / |
            //  +---+
            rotation++;
        case 0b1101:
            //  +---o
            //  | \ |
            //  +---+
            rotation++;
        case 0b1011:
            //  +---+
            //  | / |
            //  +---o
            blockType = BLOCK_3CORNER;
            break;

        // BLOCK_1CORNER + BLOCK_1CORNER
        case 0b0101:
            //  +---o
            //  | / |
            //  o---+
            rotation++;
        case 0b1010:
            //  o---+
            //  | \ |
            //  +---o

            break;
    }
    blockType++;
}

bool mapgen_load_heightmap_2(const utf8* path)
{
    auto format = Imaging::GetImageFormatFromPath(path);
    if (format == IMAGE_FORMAT::PNG)
    {
        // Promote to 32-bit
        format = IMAGE_FORMAT::PNG_32;
    }

    try
    {
        auto image = Imaging::ReadFromFile(path, format);
        if (image.Width != image.Height)
        {
            context_show_error(STR_HEIGHT_MAP_ERROR, STR_ERROR_WIDTH_AND_HEIGHT_DO_NOT_MATCH);
            return false;
        }

        auto size = image.Width;
        if (image.Width > MAXIMUM_MAP_SIZE_PRACTICAL * 2)
        {
            context_show_error(STR_HEIGHT_MAP_ERROR, STR_ERROR_HEIHGT_MAP_TOO_BIG);
            size = std::min<uint32_t>(image.Height, MAXIMUM_MAP_SIZE_PRACTICAL * 2);
        }

        // Allocate memory for the height map values, one byte pixel
        delete[] _heightMapData.mono_bitmap;
        _heightMapData.mono_bitmap = new uint8_t[size * size];
        _heightMapData.width = size;
        _heightMapData.height = size;

        // Copy average RGB value to mono bitmap
        constexpr auto numChannels = 4;
        const auto pitch = image.Stride;
        const auto pixels = image.Pixels.data();
        for (uint32_t x = 0; x < _heightMapData.width; x++)
        {
            for (uint32_t y = 0; y < _heightMapData.height; y++)
            {
                const auto red = pixels[x * numChannels + y * pitch];
                const auto green = pixels[x * numChannels + y * pitch + 1];
                const auto blue = pixels[x * numChannels + y * pitch + 2];
                _heightMapData.mono_bitmap[x + y * _heightMapData.width] = (red + green + blue) / 3;
            }
        }
        return true;
    }
    catch (const std::exception& e)
    {
        switch (format)
        {
            case IMAGE_FORMAT::BITMAP:
                context_show_error(STR_HEIGHT_MAP_ERROR, STR_ERROR_READING_BITMAP);
                break;
            case IMAGE_FORMAT::PNG_32:
                context_show_error(STR_HEIGHT_MAP_ERROR, STR_ERROR_READING_PNG);
                break;
            default:
                log_error("Unable to load height map image: %s", e.what());
                break;
        }
        return false;
    }
}
