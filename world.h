//
// Created by AJ Austinson on 12/26/2020.
//

#ifndef AGOS_WORLD_H
#define AGOS_WORLD_H

#include "pch.h"
#include "array.h"

#define TILE_FLAG_PROVINCE_SEED 1

typedef struct {
	uint16_t x, y;
	uint16_t province; 
	uint16_t flags;
	uint16_t neighbors[6];
	uint16_t shuffledNeighbors[6];
	uint16_t distFromSeed;
	float spindle;
	Vector2 p;
} WorldTile;

typedef struct {
	Array tileIndices;
	size_t regionIndex;
	uint16_t feature;
	uint16_t development;   // TODO: This needs to be its own system probably
	uint32_t population;    // TODO: This needs to be aware of various species
	Vector3 position;
	// TODO: Yields
} WorldLocation;

typedef struct {
	Array locationIndices;
	Array neighborRegions;
	size_t areaIndex;
	uint8_t biome;
	Model terrainModel;
} WorldRegion;

#define WORLD_AREA_CONTINENT 1
#define WORLD_AREA_OCEAN 2

typedef struct {
	uint8_t type;
	Array regionIndices;
} WorldArea;

typedef struct {
    unsigned int seed;
    Array tiles;
    Array locations;
    Array regions;
    Array areas;
} World;


void WorldGenerate();
void WorldDraw();

// Mouse picking
WorldTile* WorldGetTileNearestPosition(Vector3 position);
WorldLocation* WorldGetLocationNearestPosition(Vector3 position);
WorldRegion* WorldGetRegionNearestPosition(Vector3 position);


#endif //AGOS_WORLD_H
