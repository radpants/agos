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

    Camera3D cam = (Camera3D){
        .fovy = 60.f,
        .position = { 0.f, 32.f, -32.f },
        .target = { 0.f, 0.f, 0.f },
        .up = { 0.f, 1.f, 0.f },
        .type = CAMERA_PERSPECTIVE
    };

    while(!WindowShouldClose()) {
        UpdateCamera(&cam);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(cam);
        WorldDraw();
        DrawCube((Vector3){0.f, 0.f, 0.f}, 1.f, 1.f, 1.f, BLUE);
        EndMode3D();

        UIDraw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
