#include <raylib.h>
#include <raygui.h>

#include "RoadNetwork.h"

/* current notes
 * - snapping causes roads to not be intersectable? (but only for mouse?? (fixes when moved with RMB??????????? (AND ONLY HORIZONTAL LINES WHATTTT IS HAPPENINGGG))
 * - if a road point is detected on the mouse, it will override placement on nearest road intersection
 * - snapping to intersection overrides closer junction collision objections
 */


int main() {
	InitWindow(1920, 1080, "CitySimulation");
	SetTargetFPS(60);
	RoadNetwork network{};

	Vector2 screenPos{};
	float scale{ 1 };

	while (!WindowShouldClose()) {
		BeginDrawing();

		if (IsKeyPressed(KEY_A)) network.currentRoadType = ARTERIAL;
		else if (IsKeyPressed(KEY_C)) network.currentRoadType = COLLECTOR;
		else if (IsKeyPressed(KEY_L)) network.currentRoadType = LOCAL;

		network.designRoads(GetMousePosition());
		network.draw(screenPos, scale);

		ClearBackground(BLACK);

		EndDrawing();
	}

	return 0;
}
