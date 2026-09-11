#include <raylib.h>
#include <RoadNetwork.h>

#include "Simulation.h"

Simulation::Simulation(Vector2 worldSize, Vector2 screenSize, int targetFPS) {
	InitWindow((int)screenSize.x, (int)screenSize.y, "City Simulation");
	SetTargetFPS(targetFPS);

	float sidebarWidth{ 300 };
	float menubarHeight{ 100 };
	Rectangle viewport { sidebarWidth, menubarHeight, 
				screenSize.x - sidebarWidth, 
				screenSize.y - menubarHeight};

	// Vector2 minSourceSize = (Vector2){ viewport.width / 2, viewport.height / 2 };

	roadNetwork = (RoadNetwork){ worldSize };
	camera = (ViewportCamera){ viewport, worldSize };
}

void Simulation::run() {
	while (!WindowShouldClose()) {
		BeginDrawing();

		editRoads();
		camera.drawRoads(roadNetwork);
		ClearBackground(BLACK);

		camera.move();

		// DrawText(std::to_string(viewport.height / viewport.width).c_str(), 10, 100, 20, RED);
		// DrawText(std::to_string(sourceRec.height / sourceRec.width).c_str(), 10, 120, 20, RED);

		EndDrawing();
	}
}



void Simulation::editRoads() {
	if (CheckCollisionPointRec(GetMousePosition(), camera.viewport)) {
		Vector2 mousePos{ camera.screenToWorldPos(GetMousePosition()) };
		roadNetwork.designRoads(mousePos);
	}
}
