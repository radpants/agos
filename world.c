//
// Created by AJ Austinson on 12/26/2020.
//

#include "world.h"

#include "hex.h"
#include "vendor/raylib/src/external/stb_perlin.h"

static World world = {0};

void WorldGenerate() {
    static const int worldWidth = 128;
    static const float fq1 = 8.0f;
    static const float fq2 = 3.0f;

    Hex* hexes = (Hex*)calloc(worldWidth * worldWidth, sizeof(Hex));
    Vector4* hexData = (Vector4*)calloc(worldWidth * worldWidth, sizeof(Vector4));

    world.dataImage = GenImageColor(worldWidth, worldWidth, BLACK);

    Image cellImages[WORLD_CELL_DIMENSIONS*WORLD_CELL_DIMENSIONS];
    const int cellDimensions = worldWidth / WORLD_CELL_DIMENSIONS;
    for(int i = 0; i < WORLD_CELL_DIMENSIONS * WORLD_CELL_DIMENSIONS; ++i) {
        cellImages[i] = GenImageColor(cellDimensions, cellDimensions, BLACK);
    }

    for(int y = 0; y < worldWidth; ++y) {
        for(int x = 0; x < worldWidth; ++x) {
            int i = y * worldWidth + x;
            hexes[i] = HexMake(x,y);
            HexGetNeighbors(&hexes[i], worldWidth); // NOTE: Will return negative & oob indices, these are invalid
            float u = (float)x / (float)worldWidth;
            float v = (float)y / (float)worldWidth;
            float noise1 = stb_perlin_ridge_noise3(u*fq1, 0.f, v*fq1, 2.f, 0.5f, 1.f, 6);
            float noise2 = stb_perlin_noise3(u*fq2, 8.f, v*fq2, 0, 0, 0) * 0.5f + 0.5f;
            unsigned char r = (unsigned char)(noise1 * 255.f);
            unsigned char g = (unsigned char)(noise2 * 255.f);
            Color color = { r, g, r, 255 };
            ImageDrawPixel(&world.dataImage, x, y, color);
            const int cellX = x / cellDimensions;
            const int cellY = y / cellDimensions;
            const int ci = cellY * cellDimensions + cellX;
            const int cellLocalX = x % cellDimensions;
            const int cellLocalY = y % cellDimensions;
            ImageDrawPixel(&cellImages[ci], cellLocalX, cellLocalY, color);
            world.cells[ci].offset = (Vector3) {
                (float)(cellX * cellDimensions),
                0.f,
                (float)(cellY * cellDimensions)
            };
        }
    }

    const Vector3 cellSize = {
            (float)cellDimensions,
            (float)cellDimensions / 2.f,
            (float)cellDimensions
    };

    for(int i = 0; i < WORLD_CELL_DIMENSIONS * WORLD_CELL_DIMENSIONS; ++i) {
        Mesh mesh = GenMeshHeightmap(cellImages[i], cellSize);
        world.cells[i].model = LoadModelFromMesh(mesh);
        world.cells[i].texture = LoadTextureFromImage(cellImages[i]);
        world.cells[i].model.materials[0].maps[MAP_ALBEDO].texture = world.cells[i].texture;
        // TODO: Is this when this should be unloaded???
//        UnloadMesh(mesh);
        UnloadImage(cellImages[i]);
    }

    free(hexes);
    free(hexData);
}

void WorldDraw() {
    for(int i = 0; i < WORLD_CELL_DIMENSIONS * WORLD_CELL_DIMENSIONS; ++i) {
        DrawModel(world.cells[i].model, world.cells[i].offset, 1.f, YELLOW);
    }
}
