//
// Created by AJ Austinson on 12/26/2020.
//

#ifndef AGOS_WORLD_H
#define AGOS_WORLD_H

#include "pch.h"
#include "hex.h"

#define WORLD_CELL_DIMENSIONS 4

typedef struct {
    Vector3 offset;
    Model model;
    Texture2D texture;
} WorldCell;

typedef struct {
    unsigned int seed;
    WorldCell cells[WORLD_CELL_DIMENSIONS*WORLD_CELL_DIMENSIONS];
    Image dataImage;
} World;

void WorldGenerate();
void WorldDraw();

// for mouse picking
Hex* WorldGetHexNearestPosition(Vector3 position);


#endif //AGOS_WORLD_H
