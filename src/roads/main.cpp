#include <raylib.h>
#include <raygui.h>

#include "RoadNetwork.h"

int main() {
	InitWindow(1920, 1080, "CitySimulation");
	SetTargetFPS(60);
	RoadNetwork network{};

	Vector2 screenPos{};
	float scale{ 1 };

	Road newRoad{ ARTERIAL, false };
	while (!WindowShouldClose()) {
		BeginDrawing();

		if (IsKeyPressed(KEY_A))
			network.currentRoadType = ARTERIAL;
		else if (IsKeyPressed(KEY_C))
			network.currentRoadType = COLLECTOR;
		else if (IsKeyPressed(KEY_L))
			network.currentRoadType = LOCAL;

		network.designRoads(GetMousePosition(), newRoad);
		network.draw(screenPos, scale);

		ClearBackground(BLACK);

		EndDrawing();
	}

	return 0;
}
