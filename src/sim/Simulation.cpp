#include <algorithm>
#include <cmath>
#include <string>

#include <raylib.h>
#include <RoadNetwork.h>

#include "Simulation.h"
#include "MathUtils.h"

Simulation::Simulation(Vector2 worldSize, Vector2 screenSize, int targetFPS) {
	this->worldSize = worldSize;
	InitWindow((int)screenSize.x, (int)screenSize.y, "City Simulation");
	SetTargetFPS(targetFPS);

	float sidebarWidth{ 300 };
	float menubarHeight{ 100 };
	viewport = (Rectangle){ sidebarWidth, menubarHeight, 
				screenSize.x - sidebarWidth, 
				screenSize.y - menubarHeight};
	float ratio(viewport.height / viewport.width);

	sourceRec = (Rectangle){ 0, 0, viewport.width, viewport.height };
	minSourceSize = (Vector2){ viewport.width / 2, viewport.height / 2 };

	roadNetwork = (RoadNetwork){ worldSize };

	// maxWorldScale = std::min((float)worldSize.x / viewport.width,
	// 			 (float)worldSize.y / viewport.height);
}

void Simulation::run() {
	while (!WindowShouldClose()) {
		BeginDrawing();

		editRoads();

		// Vector2 worldBounds{ getViewportBounds(worldScale) };
		// DrawText(std::to_string(viewport.width*worldScale).c_str(), 30, 30, 30, RED);
		// DrawText(std::to_string(worldPos.x + viewport.width*worldScale).c_str(), 30, 60, 30, RED);
		// DrawCircleV(VecMath::add(screenToWorldPos(GetMousePosition()), Vector2{viewport.x, viewport.y}), 20, GREEN);
		ClearBackground(BLACK);

		moveCamera();
		DrawText(std::to_string(viewport.height / viewport.width).c_str(), 10, 100, 20, RED);
		DrawText(std::to_string(sourceRec.height / sourceRec.width).c_str(), 10, 120, 20, RED);

		EndDrawing();
	}
}

float Simulation::zoomFactor() {
	return sourceRec.width / viewport.width;
}

Vector2 Simulation::screenToWorldPos(Vector2 pos) {
	return (Vector2){  sourceRec.x + (pos.x - viewport.x) * (sourceRec.width / viewport.width),
			   sourceRec.y + (pos.y - viewport.y) * (sourceRec.height / viewport.height) };
}

void Simulation::cameraInputHandling() {
	Vector2 delta{ VecMath::scale(GetMouseDelta(), 0.1f) };
	
	if (IsKeyDown(KEY_EQUAL) && IsKeyDown(KEY_LEFT_SHIFT)) zoomDirection = -1;
	else if (IsKeyDown(KEY_MINUS)) zoomDirection = 1;
	else zoomDirection = 0;

	if (IsMouseButtonDown(1)) moveDirection = (Vector2){ -delta.x, -delta.y };
	else {
		moveDirection = Vector2{0,0};

		if (IsKeyDown(KEY_UP)) moveDirection.y = -1; 
		else if (IsKeyDown(KEY_DOWN)) moveDirection.y = 1;
		else moveDirection.y = 0;

		if (IsKeyDown(KEY_LEFT)) moveDirection.x = -1; 
		else if (IsKeyDown(KEY_RIGHT)) moveDirection.x = 1;
		else moveDirection.x = 0;
	}

	isSprinting = IsKeyDown(KEY_LEFT_CONTROL);
}

void Simulation::zoom() {
	float mouseWheel{ (float)GetMouseWheelMove() };
	float scrollSpeed{ mouseWheel != 0 ? mouseWheel * 10.0f : 
					     zoomDirection * 10.0f };
	scrollSpeed *= zoomFactor();
	Vector2 newSrcPos{ sourceRec.x - scrollSpeed/2, sourceRec.y - scrollSpeed/2 };
	float newSrcWidth = sourceRec.width + scrollSpeed;
	float newSrcHeight = newSrcWidth * (viewport.height / viewport.width);

	bool canExpandLeft{}; bool canExpandRight{};
	bool canExpandUp{}; bool canExpandDown{};

	if (scrollSpeed > 0) {
		if (newSrcPos.x >= 0) sourceRec.x = newSrcPos.x;
		if (newSrcWidth + sourceRec.x <= worldSize.x) sourceRec.width = newSrcWidth;

		if (newSrcPos.y >= 0) sourceRec.y = newSrcPos.y;
		if (newSrcHeight + sourceRec.y <= worldSize.y) sourceRec.height = newSrcHeight;
	} else {
		sourceRec.x = std::max(0.0f, sourceRec.x - scrollSpeed/2);
		sourceRec.y = std::max(0.0f, sourceRec.y - scrollSpeed/2);
		sourceRec.width = std::max(minSourceSize.x, newSrcWidth);
		sourceRec.height = std::max(minSourceSize.y, newSrcHeight);
	}
}

void Simulation::moveCamera() {
	cameraInputHandling();

	float scrollSpeed{ float(isSprinting ? sprintStep : moveStep) };
	Vector2 moveSpeed{ VecMath::scale(moveDirection, scrollSpeed) };

	Vector2 newPos{ sourceRec.x + moveSpeed.x, sourceRec.y + moveSpeed.y };
	if (newPos.x < 0) newPos.x = 0;
	if (newPos.x + sourceRec.width > worldSize.x) newPos.x = worldSize.x - sourceRec.width;

	newPos.x = std::max(0.0f, newPos.x);
	newPos.x = std::min(newPos.x, worldSize.x - sourceRec.width);

	newPos.y = std::max(0.0f, newPos.y);
	newPos.y = std::min(newPos.y, worldSize.y - sourceRec.height);

	sourceRec.x = newPos.x; 
	sourceRec.y = newPos.y;

	zoom();
}

void Simulation::editRoads() {
	if (CheckCollisionPointRec(GetMousePosition(), viewport)) {
		Vector2 mousePos{ screenToWorldPos(GetMousePosition()) };
		roadNetwork.designRoads(mousePos);
	}

	roadNetwork.draw(sourceRec, viewport);
}
