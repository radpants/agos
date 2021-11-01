//
// Created by AJ Austinson on 12/26/2020.
//

#include "world.h"

#include "vendor/rnd.h"

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
static uint8_t IsWorldGenerated = 0;
static Texture atlas = {0};

void WorldGenerate() {
	if(IsWorldGenerated)
		return;
	IsWorldGenerated = 1;
	
	rnd_pcg_t pcg;
	rnd_pcg_seed(&pcg, 333);

	atlas = LoadTexture("data/images/hex_tiles.png");

	const uint16_t WorldSize = 128;

	// We'll need to not use uint16's if this is the case
	assert(WorldSize < 256);

	world.tiles = ArrayMake(sizeof(WorldTile));

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
			tile.height = 0.f;
			tile.seed = rnd_pcg_next(&pcg);
			for (int n = 0; n < 6; ++n) { tile.neighbors[n] = UINT16_MAX; }
			for (int n = 0; n < 6; ++n) { tile.shuffledNeighbors[n] = UINT16_MAX; }
			ArrayPush(&world.tiles, &tile);
		}
	} 

	WorldTile* tiles = (WorldTile*)world.tiles.data;

	for (uint16_t i = 0; i < world.tiles.count; ++i) {
		if (tiles[i].y > 0) {
			if(tiles[i].x + (tiles[i].y & 1) > 0)
				tiles[i].neighbors[0] = i - WorldSize - ((tiles[i].y+1) & 1);			// NW
			if(tiles[i].x - (tiles[i].y & 1) < WorldSize - 1)
				tiles[i].neighbors[1] = i - WorldSize - ((tiles[i].y+1) & 1) + 1;		// NE
		}
		if (tiles[i].x < WorldSize - 1)
			tiles[i].neighbors[2] = i + 1;		// E
		if (tiles[i].y < WorldSize - 1) {
			if(tiles[i].x + (tiles[i].y & 1) > 0)
				tiles[i].neighbors[4] = i + WorldSize - ((tiles[i].y+1) & 1);			// SE
			if(tiles[i].x - (tiles[i].y & 1) < WorldSize - 1)
				tiles[i].neighbors[3] = i + WorldSize - ((tiles[i].y+1) & 1) + 1;		// SW
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
 
	static const int ProvinceSpacing = 8;
	static const float Frequency = 4.f;
	uint16_t provinceIndex = 0;
	int unclaimedTiles = world.tiles.count;
	for (uint16_t y = ProvinceSpacing/2; y < WorldSize - ProvinceSpacing/2; y += ProvinceSpacing) { 
		for (uint16_t x = ProvinceSpacing/2; x < WorldSize - ProvinceSpacing/2; x += ProvinceSpacing) { 
			uint16_t idx = tiles[y * WorldSize + x].shuffledNeighbors[0];
			idx = tiles[idx].shuffledNeighbors[0];
			idx = tiles[idx].shuffledNeighbors[0];
			tiles[idx].province = provinceIndex++;
			tiles[idx].distFromSeed = 0;
			tiles[idx].flags |= TILE_FLAG_PROVINCE_SEED;

			Vector2 uv = (Vector2){ 
				(float)x / (float)WorldSize * Frequency,
				(float)y / (float)WorldSize * Frequency
			};
			tiles[idx].height = stb_perlin_noise3(uv.x, uv.y, 0.f, 0, 0, 0) + .5f;
			tiles[idx].height = Clamp(tiles[idx].height, 0.f, 1.f);
				 
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
					tiles[ni].height = tiles[i].height; 
					counts[tiles[i].province]++;
					unclaimedTiles--;
				} 
			}
		} 
		++seedDist;
	} 

	for (uint16_t i = 0; i < world.tiles.count; ++i) {
		if(tiles[i].height >= .5f)
			tiles[i].flags |= TILE_FLAG_LAND;
		if(tiles[i].height >= .9f)
			tiles[i].flags |= TILE_FLAG_MOUNTAIN;
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
	free(edgeCounts);
	free(counts);

	for (uint16_t i = 0; i < world.tiles.count; ++i) {
		tiles[i].spindle = provinceSpindles[tiles[i].province];
	}

	free(provinceSpindles); 

	for (int i = 0; i < world.tiles.count; ++i) {
		for (int n = 0; n < 6; ++n) {
			if (tiles[i].neighbors[n] == UINT16_MAX)
				continue;
			if (tiles[i].flags & TILE_FLAG_LAND && (tiles[tiles[i].neighbors[n]].flags & TILE_FLAG_LAND) == 0) {
				tiles[i].flags |= TILE_FLAG_COASTAL;
				continue;
			}
			if (tiles[tiles[i].neighbors[n]].flags & TILE_FLAG_LAND && (tiles[i].flags & TILE_FLAG_LAND) == 0) {
				tiles[i].flags |= TILE_FLAG_COASTAL;
				continue;
			}
		}
	}

	int riversToAdd = world.tiles.count / 500;
	int totalRivers = 0;
	size_t* startIndices = (size_t*)malloc(sizeof(size_t)*riversToAdd);
	size_t start_i = 0;
	while(riversToAdd > 0) {
		int i = rnd_pcg_next(&pcg) % world.tiles.count;
		if(tiles[i].flags & TILE_FLAG_MOUNTAIN) { 
			startIndices[start_i++] = i;
			riversToAdd--;
			totalRivers++;
		}
	}

	for(int i = 0; i < totalRivers; i++) {
		tiles[startIndices[i]].flags |= TILE_FLAG_RIVER;
		tiles[startIndices[i]].riverId = i;
		WorldTile* next = &tiles[startIndices[i]];
		int dir = rnd_pcg_next(&pcg) % 6;
		int changeDir = rnd_pcg_next(&pcg) % 5;
		while(next && (next->flags & TILE_FLAG_LAND)) {
			if(changeDir-- <= 0) {
				dir += (rnd_pcg_next(&pcg) % 2) ? -1 : 1;
				dir = dir % 6;
				changeDir = rnd_pcg_next(&pcg) % 6;
			}
			next->flags |= TILE_FLAG_RIVER;
			next->riverId = i;
			if( next->neighbors[dir] != UINT16_MAX ) {
				WorldTile* prev = next;
				next = &tiles[next->neighbors[dir]];
				if(next->flags & TILE_FLAG_RIVER && next->riverId != i) {
					next->flags |= TILE_FLAG_TRIBUTARY;
					prev->flags |= TILE_FLAG_TRIBUTARY;
					break;
				}
			}
			else
				changeDir = 0;
		}
	}

	free(startIndices);
	

	world.dataImage = GenImageColor(WorldSize, WorldSize, BLACK);

	for(int y = 0; y < WorldSize; ++y) {
		for(int x = 0; x < WorldSize; ++x) {
			size_t i = y * WorldSize + x;
			float hue = (1.f - (float)tiles[i].height) * 360.f;
			float sat = tiles[i].height > 0.5f ? 1.f : .2f;
			Color c = ColorFromHSV(hue, sat, .8f);
			ImageDrawPixel(&world.dataImage, x, y, c);
		}
	}
	
	world.dataTexture = LoadTextureFromImage(world.dataImage); 
}

void WorldDraw() {
	if(!IsWorldGenerated) 
		return;

	WorldTile* tiles = (WorldTile*)world.tiles.data;

	static const Rectangle src = { 0, 0, 32, 27 };
	static const Rectangle poi_src = { 495, 28, 32, 27 };
	static Rectangle edge_src[] = {
		{ 33, 252, 16, 9 },
		{ 49, 252, 16, 9 },
		{ 64, 259, 1, 12 },
		{ 49, 270, 16, 9 },
		{ 33, 270, 16, 9 },
		{ 33, 259, 1, 12 }
	};

	char buf[32];
	for (int i = 0; i < world.tiles.count; ++i) {

		tiles[i].p = (Vector2){
			.x = tiles[i].x * 32 + ((tiles[i].y&1) * 16),
			.y = tiles[i].y * 19,
		};


		if( tiles[i].flags & TILE_FLAG_LAND ) {
			Rectangle tile_src = src;

			if (tiles[i].flags & TILE_FLAG_PROVINCE_SEED) {
				float hue = (1.f - (float)tiles[i].height) * 360.f;
				float sat = tiles[i].height > 0.5f ? 1.f : .2f;
				Color c = ColorFromHSV(hue, sat, .8f);
				DrawTextureRec(atlas, poi_src, tiles[i].p, WHITE);

				sprintf((char * restrict)&buf, "%.3f", tiles[i].height);
				int w = MeasureText(buf, 10);
				DrawText(buf, tiles[i].p.x - (w/2) + 16, tiles[i].p.y - 15, 10, c);

				sprintf((char * restrict)&buf, "%.3f", tiles[i].spindle);
				w = MeasureText(buf, 10);
				DrawText(buf, tiles[i].p.x - (w/2) + 16, tiles[i].p.y - 25, 10, GRAY); 
			}
			else if(tiles[i].flags & TILE_FLAG_RIVER) { 
				uint8_t sig = 0;
				for(int n = 0; n < 6; n++) {
					if(tiles[i].neighbors[n] == UINT16_MAX)
						continue;
					if(tiles[tiles[i].neighbors[n]].flags & TILE_FLAG_RIVER) {
						if(tiles[tiles[i].neighbors[n]].riverId == tiles[i].riverId || tiles[i].flags & TILE_FLAG_TRIBUTARY) {
							sig |= (1 << n);
						}
					}
				}
				if(sig > 0) {
					int sx = 33 + ((sig-1) * 33);
					int sy = 84;
					if(sx >= 2048 - 33) {
						sx -= 2048;
						sy += 28;
					}
					Rectangle river_src = { sx, sy, 32, 27 };
					DrawTextureRec(atlas, river_src, tiles[i].p, WHITE);
					sprintf((char * restrict)&buf, "%d", tiles[i].riverId);
					DrawText(buf, tiles[i].p.x + 10, tiles[i].p.y + 8, 10, WHITE);
				}
			}
			else if(tiles[i].flags & TILE_FLAG_COASTAL) {
				uint8_t sig = 0;
				for(int n = 0; n < 6; n++) {
					if(tiles[i].neighbors[n] == UINT16_MAX)
						continue;
					if((tiles[tiles[i].neighbors[n]].flags & TILE_FLAG_LAND) == 0) {
						sig |= (1 << n);
					}
				}
				if(sig > 0) {
					int sx = 66 + ((sig-1) * 33);
					int sy = 112;
					if(sx >= 2048 - 32) {
						sx -= 2048;
						sy += 28;
					}
					Rectangle coast_src = { sx, sy, 32, 27 };
					DrawTextureRec(atlas, coast_src, tiles[i].p, WHITE);
				} 
			}
			else {
				if(tiles[i].flags & TILE_FLAG_MOUNTAIN)
					tile_src.x = 330 + 33 * (tiles[i].seed % 6);
				else
					tile_src.x = 33 * (tiles[i].seed % 10);

				DrawTextureRec(atlas, tile_src, tiles[i].p, WHITE); 
			}
		}
	}

	Color ecol = GetColor(0xffffff33);
	for (int i = 0; i < world.tiles.count; ++i) {
		for (int n = 0; n < 3; ++n) {
			if (tiles[i].neighbors[n] == UINT16_MAX)
				continue;
			if (tiles[i].flags & TILE_FLAG_COASTAL)
				continue;
			if (tiles[tiles[i].neighbors[n]].province == tiles[i].province)
				continue;

			DrawTextureRec(atlas, edge_src[n], (Vector2){ tiles[i].p.x + (edge_src[n].x - 33), tiles[i].p.y + (edge_src[n].y - 252) }, ecol);
		}
	} 
	
	//DrawTextureEx(world.dataTexture, (Vector2) { 0 }, 0.f, 3.f, WHITE);
}
