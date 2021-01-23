//
// Created by AJ Austinson on 12/26/2020.
//

#ifndef AGOS_WORLD_H
#define AGOS_WORLD_H

#include "pch.h"

DA_TYPEDEF(size_t, IndexArray)

typedef struct {
	uint16_t x, y;
	uint32_t data;
} WorldTile;

DA_TYPEDEF(WorldTile, WorldTileArray)


typedef struct {
	IndexArray tileIndices;
	size_t regionIndex;
	uint16_t feature;
	uint16_t development;   // TODO: This needs to be its own system probably
	uint32_t population;    // TODO: This needs to be aware of various species
	Vector3 position;
	// TODO: Yields
} WorldLocation;

DA_TYPEDEF(WorldLocation, WorldLocationArray);


typedef struct {
	IndexArray locationIndices;
	IndexArray neighborRegions;
	size_t areaIndex;
	uint8_t biome;
	Model terrainModel;
} WorldRegion;

DA_TYPEDEF(WorldRegion, WorldRegionArray);


#define WORLD_AREA_CONTINENT 1
#define WORLD_AREA_OCEAN 2

typedef struct {
	uint8_t type;
	IndexArray regionIndices;
} WorldArea;

DA_TYPEDEF(WorldArea, WorldAreaArray);


typedef struct {
    unsigned int seed;
    WorldTileArray tiles;
    WorldLocationArray locations;
    WorldRegionArray regions;
    WorldAreaArray areas;
} World;


void WorldGenerate();
void WorldDraw();

// Mouse picking
WorldTile* WorldGetTileNearestPosition(Vector3 position);
WorldLocation* WorldGetLocationNearestPosition(Vector3 position);
WorldRegion* WorldGetRegionNearestPosition(Vector3 position);


#endif //AGOS_WORLD_H
