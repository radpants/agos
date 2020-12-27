//
// Created by AJ Austinson on 12/26/2020.
//

#include "world.h"

#include "hex.h"
#include "vendor/raylib/src/external/stb_perlin.h"

static World world = {0};

// DEBUG
static Texture2D debugTexture;
// END DEBUG

void WorldGenerate() {
    static const int worldWidth = 128;
    static const float fq1 = 8.0f;
    static const float fq2 = 3.0f;

    Hex* hexes = (Hex*)calloc(worldWidth * worldWidth, sizeof(Hex));
    Vector4* hexData = (Vector4*)calloc(worldWidth * worldWidth, sizeof(Vector4));

    world.dataImage = GenImageColor(worldWidth, worldWidth, BLACK);

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
            ImageDrawPixel(&world.dataImage, x, y, (Color){ r, g, 0, 255 });
        }
    }

    free(hexes);
    free(hexData);

    debugTexture = LoadTextureFromImage(world.dataImage);
}

void WorldDraw() {
    SetTextureFilter(debugTexture, FILTER_POINT);
    DrawTextureEx(debugTexture, (Vector2){ 128.f, 128.f }, 0.f, 4.f, WHITE);
}
