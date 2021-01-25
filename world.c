//
// Created by AJ Austinson on 12/26/2020.
//

#include "world.h"

#include <rnd.h>

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

	// We'll need to not use uint16's if this is the case
	assert(WorldSize < 256);

	world.tiles = ArrayMake(sizeof(WorldTile));
	world.locations = ArrayMake(sizeof(WorldLocation));
	world.regions = ArrayMake(sizeof(WorldRegion));
	world.areas = ArrayMake(sizeof(WorldArea)); 

	ArrayResize(&world.tiles, WorldSize * WorldSize); 
	
	for(uint16_t y = 0; y < WorldSize; y++ ){
		for(uint16_t x = 0; x < WorldSize; x++ ) {
			WorldTile tile;
			tile.x = x;
			tile.y = y;
			tile.flags = 0;
			tile.province = UINT16_MAX;
			tile.distFromSeed = UINT16_MAX;
			tile.spindle = 0.f;
			for (int n = 0; n < 6; ++n) { tile.neighbors[n] = UINT16_MAX; }
			for (int n = 0; n < 6; ++n) { tile.shuffledNeighbors[n] = UINT16_MAX; }
			ArrayPush(&world.tiles, &tile);
		}
	} 

	WorldTile* tiles = (WorldTile*)world.tiles.data;

	rnd_pcg_t pcg;
	rnd_pcg_seed(&pcg, 1234);

	for (uint16_t i = 0; i < world.tiles.count; ++i) {
		if (tiles[i].y > 0) { 
			if(tiles[i].x + (tiles[i].y & 1) > 0)
				tiles[i].neighbors[0] = i - WorldSize - (tiles[i].y & 1);			// NW
			if(tiles[i].x - (tiles[i].y & 1) < WorldSize)
				tiles[i].neighbors[1] = i - WorldSize - (tiles[i].y & 1) + 1;		// NE
		}
		if (tiles[i].x < WorldSize)
			tiles[i].neighbors[2] = i + 1;		// E
		if (tiles[i].y < WorldSize) {
			if(tiles[i].x + (tiles[i].y & 1) > 0)
				tiles[i].neighbors[3] = i + WorldSize - (tiles[i].y & 1);			// SW
			if(tiles[i].x - (tiles[i].y & 1) < WorldSize)
				tiles[i].neighbors[4] = i + WorldSize - (tiles[i].y & 1) + 1;		// SE 
		}
		if (tiles[i].x > 0)
			tiles[i].neighbors[5] = i - 1;		// W

		for (int n = 0; n < 6; ++n) { 
			tiles[i].shuffledNeighbors[n] = tiles[i].neighbors[n];
		}
		for (int n = 0; n < 12; ++n) { 
			int a = rnd_pcg_next(&pcg) % 6;
			int b = rnd_pcg_next(&pcg) % 6;
			uint16_t temp = tiles[i].shuffledNeighbors[a];
			tiles[i].shuffledNeighbors[a] = tiles[i].shuffledNeighbors[b];
			tiles[i].shuffledNeighbors[b] = temp;
		}

	}
 
	static const ProvinceSpacing = 6;
	uint16_t provinceIndex = 0;
	int unclaimedTiles = world.tiles.count;
	for (uint16_t y = ProvinceSpacing; y < WorldSize - ProvinceSpacing; y += ProvinceSpacing) { 
		for (uint16_t x = ProvinceSpacing; x < WorldSize - ProvinceSpacing; x += ProvinceSpacing) { 
			uint16_t idx = tiles[y * WorldSize + x].shuffledNeighbors[0]; 
			idx = tiles[idx].shuffledNeighbors[0];
			tiles[idx].province = provinceIndex++;
			tiles[idx].distFromSeed = 0;
			tiles[idx].flags |= TILE_FLAG_PROVINCE_SEED;
			unclaimedTiles--;
		}
	}
 
	uint16_t* counts = (uint16_t*)malloc(sizeof(uint16_t) * provinceIndex);
	uint16_t* edgeCounts = (uint16_t*)malloc(sizeof(uint16_t) * provinceIndex);

	for (int i = 0; i < provinceIndex; ++i) { 
		counts[i] = 1;
		edgeCounts[i] = 0;
	}

	uint16_t seedDist = 0;
	while (unclaimedTiles > 0) { 
		for (uint16_t i = 0; i < world.tiles.count; ++i) {
			if (tiles[i].province == UINT16_MAX)
				continue; 
			if (tiles[i].distFromSeed != seedDist)
				continue; 
			for (int n = 0; n < 6; ++n) {
			//uint16_t n = rnd_pcg_next(&pcg) % 6;
				uint16_t ni = tiles[i].shuffledNeighbors[n];
				if (ni != UINT16_MAX && tiles[ni].province == UINT16_MAX) { 
					tiles[ni].province = tiles[i].province;
					tiles[ni].distFromSeed = tiles[i].distFromSeed + 1;
					counts[tiles[i].province]++;
					unclaimedTiles--;
				} 
			}
		} 
		++seedDist;
	} 

	for (uint16_t i = 0; i < world.tiles.count; ++i) {
		for (int n = 0; n < 6; ++n) { 
			if (tiles[i].neighbors[n] == UINT16_MAX)
				continue;
			if (tiles[tiles[i].neighbors[n]].province != tiles[i].province) {
				edgeCounts[tiles[i].province]++;
				break;
			}
		}
	}

	float* provinceSpindles = (float*)malloc(sizeof(float) * provinceIndex); 
	for (uint16_t i = 0; i < provinceIndex; ++i) { 
		provinceSpindles[i] = (float)edgeCounts[i] / (float)counts[i];
	} 

	for (uint16_t i = 0; i < world.tiles.count; ++i) {
		tiles[i].spindle = provinceSpindles[tiles[i].province];
	}

	free(counts);
	free(edgeCounts);
	free(provinceSpindles); 
}

void WorldDraw() { 
	WorldTile* tiles = (WorldTile*)world.tiles.data;
	static const float TileSize = 15.f;
	static const float Sqrt3 = 1.73205f;
	static const float t1 = -(3.14159f / 3.f) * 2.5f;
	static const float t2 = -(3.14159f / 3.f) * 1.5f;
	static const float t3 = -(3.14159f / 3.f) * 0.5f;
	static const float t4 = (3.14159f / 3.f) * 0.5f;
	const float ts[] = { t1, t2, t3, t4 };
	const Color cs[] = { YELLOW, RED, SKYBLUE, MAGENTA };
	const Color lineCol = GetColor(0x222222ff);

	for (int i = 0; i < world.tiles.count; ++i) {
		//int px = tiles[i].x * TileSize + ((tiles[i].y&1) * TileSize / 2);
		//int py = tiles[i].y * TileSize; 
		Vector2 p = { 0 };
		p.x = TileSize * Sqrt3 * ((float)tiles[i].x - .5f * (float)(tiles[i].y & 1));
		p.y = TileSize * 3.f / 2.f * (float)tiles[i].y;
		tiles[i].p = p;
		DrawPolyLines(p, 6, TileSize, 0.f, lineCol); 

		if (tiles[i].flags & TILE_FLAG_PROVINCE_SEED) {
			DrawCircle(p.x, p.y, 4.f, GRAY);
		}
		else if (tiles[i].spindle >= 0.7f) {
			DrawPoly(p, 3, 4.f, 60.f, DARKGRAY);
		}
	}

	for (int i = 0; i < world.tiles.count; ++i) { 
		for (int n = 0; n < 3; ++n) {
			if (tiles[i].neighbors[n] == UINT16_MAX)
				continue;
			if (tiles[tiles[i].neighbors[n]].province == tiles[i].province)
				continue;

			Vector2 a = {
				tiles[i].p.x + TileSize * cos(ts[n]),
				tiles[i].p.y + TileSize * sin(ts[n])
			};

			Vector2 b = {
				tiles[i].p.x + TileSize * cos(ts[n+1]),
				tiles[i].p.y + TileSize * sin(ts[n+1])
			};

			DrawLineEx(a, b, 2.f, RAYWHITE); 
		}
	}
}
