#include <raylib.h>
#include <RoadNetwork.h>
#include <MathUtils.h>
#include "Simulation.h"

#include <iostream>

ViewportCamera::ViewportCamera(Rectangle viewport, Vector2 worldSize) {
	this->viewport = viewport;
	this->worldSize = worldSize; 

	sourceRec = (Rectangle){ 0, 0, viewport.width, viewport.height };
	minSourceSize = (Vector2){ viewport.width / 2, viewport.height / 2 };
}

float ViewportCamera::zoomFactor() {
	return sourceRec.width / viewport.width;
}

Vector2 ViewportCamera::screenToWorldPos(Vector2 pos) {
	return (Vector2){  sourceRec.x + (pos.x - viewport.x) * (sourceRec.width / viewport.width),
			   sourceRec.y + (pos.y - viewport.y) * (sourceRec.height / viewport.height) };
}

void ViewportCamera::inputHandling() {
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

void ViewportCamera::zoom() {
	float mouseWheel{ (float)GetMouseWheelMove() };
	float speed{ zoomSpeed * (isSprinting ? sprintFactor : 1.0f) };
	float scrollSpeed{ mouseWheel != 0 ? mouseWheel * speed: 
					     zoomDirection * speed };
	scrollSpeed *= zoomFactor();


	Vector2 newSrcPos{ sourceRec.x - scrollSpeed/2, sourceRec.y - zoomFactor()*scrollSpeed/2 };
	float newSrcWidth = sourceRec.width + scrollSpeed;
	float newSrcHeight = newSrcWidth * (viewport.height / viewport.width);

	if (scrollSpeed > 0) {
		if (newSrcPos.x >= 0) sourceRec.x = newSrcPos.x;
		if (newSrcWidth + sourceRec.x <= worldSize.x) sourceRec.width = newSrcWidth;

		if (newSrcPos.y >= 0) sourceRec.y = newSrcPos.y;
		if (newSrcHeight + sourceRec.y <= worldSize.y) sourceRec.height = newSrcHeight;
	} else {
		sourceRec.width = std::max(minSourceSize.x, newSrcWidth);
		sourceRec.height = std::max(minSourceSize.y, newSrcHeight);

		if (!(sourceRec.width == minSourceSize.x) && !(sourceRec.height == minSourceSize.y)) {
			sourceRec.x = std::max(0.0f, newSrcPos.x);
			sourceRec.y = std::max(0.0f, newSrcPos.y);
		}
	}
}

void ViewportCamera::move() {
	inputHandling();

	float scrollSpeed{ float(isSprinting ? sprintFactor : 1.0f) * moveSpeed };
	Vector2 moveSpeed{ VecMath::scale(moveDirection, scrollSpeed) };

	Vector2 newPos{ sourceRec.x + moveSpeed.x, sourceRec.y + moveSpeed.y };

	newPos.x = std::max(0.0f, newPos.x);
	newPos.x = std::min(newPos.x, worldSize.x - sourceRec.width);

	newPos.y = std::max(0.0f, newPos.y);
	newPos.y = std::min(newPos.y, worldSize.y - sourceRec.height);

	sourceRec.x = newPos.x; 
	sourceRec.y = newPos.y;

	zoom();
}

void ViewportCamera::drawRoads(RoadNetwork roadNetwork) {
	Rectangle srcRec = (Rectangle){ sourceRec.x, sourceRec.y, sourceRec.width, sourceRec.height }; 

	srcRec.y = -srcRec.y - srcRec.height;
	srcRec.height *= -1;

	Vector2 pos{ viewport.x, viewport.y };

	DrawTexturePro(roadNetwork.roadTexture.texture, srcRec, viewport, Vector2{0.0f, 0.0f}, 0.0f, RAYWHITE);
	DrawTexturePro(roadNetwork.uiTexture.texture, srcRec, viewport, Vector2{0.0f, 0.0f}, 0.0f, RAYWHITE);

	DrawText(std::to_string(sourceRec.x).c_str(), 10, 100, 20, RED);

	DrawRectangleLinesEx(viewport, 5, RAYWHITE);
	DrawFPS(pos.x + 10, pos.y + 10);
}
