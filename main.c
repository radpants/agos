#include "pch.h"

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
			}

			pMouse = mouse;
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
		ClearBackground(RAYWHITE);

		BeginMode3D(cam);
		WorldDraw();
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
