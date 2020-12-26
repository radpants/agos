#include <raylib.h>

int main() {
    InitWindow(1280, 720, "Agos");
    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Agos!", 32, 32, 16, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
