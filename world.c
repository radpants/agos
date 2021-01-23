//
// Created by AJ Austinson on 12/26/2020.
//

#include "world.h"

#include "vendor/raylib/src/external/stb_perlin.h"

/*
 * Okay, what kind of information does the world map need to store?
 * - Tiles (pixels in the world map data)
 * - Locations (groups of Tiles)
 * - Regions (groups of Locations)
 * - Continents (groups of Regions that are land based)
 * - Oceans (groups of Regions that are ocean based)
 * - Resources / Sites (this is attached to a Location)
 * - Settlements (attached to a Location, referenced by Region maybe?)
 * - Population (attached to Locations)
 * - Yields (attached to Locations, calculated up to Regions, then up to Nation)
 * - Navigation (Region level, which Regions are connected to other regions)
 * - Detail Props (Trees, etc. Attached to Region, but calculated using Tile)
 * - Location Prop (Signifies what this Location is, if anything)
 * - AABB (Attached to Region)
 * - Region Model (Attached to Region, this is the base terrain mesh model)
 */

static World world = {0};

void WorldGenerate() {
	const uint16_t WorldSize = 128;
	
	da_init(world.tiles);
	da_init(world.locations);
	da_init(world.regions);
	da_init(world.areas);
	
	da_reserve(world.tiles, WorldSize * WorldSize);
	
	for(uint16_t y = 0; y < WorldSize; y++ ){
		for(uint16_t x = 0; x < WorldSize; x++ ) {
			WorldTile tile;
			tile.x = x;
			tile.y = y;
			tile.data = 0;
			da_add(world.tiles, tile);
		}
	}
	
	IndexArray startingTileIndices = {0};
	for(uint16_t y = 0; y < WorldSize; y++ ) {
		for (uint16_t x = 0; x < WorldSize; x++) {
			if( x % 16 == 0 &&  y % 16 == 0 ) {
				da_add(startingTileIndices, y * WorldSize + x);
			}
		}
	}
	
	
	
}

void WorldDraw() {
}
