#include "pch.h"

#include "ui.h"
#include "world.h"

void HandleGenerate() {
    WorldGenerate();
}

int main() {
    InitWindow(1280, 720, "Agos");
    SetTargetFPS(60);

    UISetClickGenerateCallback(&HandleGenerate);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        WorldDraw();
        UIDoFrame();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
