#include "pch.h"

#define RND_IMPLEMENTATION
#include <rnd.h>

#include "ui.h"
#include "world.h"

void HandleGenerate() {
	WorldGenerate();
}

int main() {
	InitWindow(1920, 1080, "Agos");
	SetTargetFPS(60);

	UISetClickGenerateCallback(&HandleGenerate);

	Vector2 pMouse = {0.f, 0.f};
	
	const float MinCamDist = 25.f;
	const float MaxCamDist = 150.f;
	float camDist = MinCamDist;
	float camDistGoal = camDist;
	const Vector3 ZoomedOutVector = {0.f, 1.f, 0.f};
	const Vector3 ZoomedInVector = Vector3Normalize((Vector3){0.f, 0.7f, 1.f});

	Camera3D cam = (Camera3D){
		.fovy = 60.f,
		.position = { 0.f, 32.f, -16.f },
		.target = { 0.f, 0.f, 0.f },
		.up = { 0.f, 1.f, 0.f },
		.type = CAMERA_PERSPECTIVE
	};

	Camera2D cam2d = (Camera2D){
		.offset = { 0.f, 0.f },
		.target = { 0.f, 0.f },
		.rotation = 0.f,
		.zoom = 1.f
	};

	while(!WindowShouldClose()) {

		if( IsMouseButtonDown(MOUSE_LEFT_BUTTON) ) {
			Vector2 mouse = GetMousePosition();
			Ray ray = GetMouseRay(GetMousePosition(), cam);
			RayHitInfo hit = GetCollisionRayGround(ray, 0.f);

			if( !IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ) {
				Ray pRay = GetMouseRay(pMouse, cam);
				RayHitInfo pHit = GetCollisionRayGround(pRay, 0.f);
				Vector3 move = Vector3Subtract(pHit.position, hit.position);
				cam.target = Vector3Add(cam.target, move);

				Vector2 move2d = Vector2Subtract(pMouse, mouse);
				cam2d.target = Vector2Add(cam2d.target, move2d);
			}

			pMouse = mouse; 
		}

		if (IsKeyPressed(KEY_G)) {
			WorldGenerate();
		}
		
		float mouseWheel = GetMouseWheelMove() * 4.f;
		camDistGoal -= mouseWheel;
		camDistGoal = (camDistGoal > MaxCamDist) ? MaxCamDist : ((camDistGoal < MinCamDist) ? MinCamDist : camDistGoal);
		camDist = (camDist + camDistGoal) / 2.f;
		float zoomFactor = (camDist - MinCamDist) / (MaxCamDist - MinCamDist);
		zoomFactor = (zoomFactor > 1.f) ? 1.f : ((zoomFactor < 0.f) ? 0.f : zoomFactor);
		Vector3 camVector = Vector3Lerp(ZoomedInVector, ZoomedOutVector, zoomFactor);
		cam.position = Vector3Add(cam.target, Vector3Scale(camVector, camDist));
		
		UpdateCamera(&cam);

		BeginDrawing();
		ClearBackground(BLACK);

		BeginMode2D(cam2d);
		WorldDraw(); 
		EndMode2D();

		BeginMode3D(cam);
		EndMode3D();

		UIDraw();
		
		char buf[16];
		sprintf(buf, "%.2f", camDist);
		DrawText(buf, 8, 128, 16, BLACK);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
