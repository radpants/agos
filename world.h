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
	float height;
} WorldTile;

typedef struct {
    Array tiles;
    Image dataImage;
    Texture2D dataTexture;
} World;


void WorldGenerate();
void WorldDraw();

// Mouse picking
WorldTile* WorldGetTileNearestPosition(Vector3 position);

#endif //AGOS_WORLD_H
