#include <raylib.h>
// #include <raygui.h>
#include "RoadNetwork.h"

/* current notes
 * - snapping causes roads to not be intersectable? (but only for mouse?? (fixes when moved with RMB??????????? (AND ONLY HORIZONTAL LINES WHATTTT IS HAPPENINGGG)) fixed
 * - if a road point is detected on the mouse, it will override placement on nearest road intersection fixed
 * - if a junction is detected near the mouse, visually correct but placement does not allign once created fixed
 * - snapping to intersection overrides closer junction collision objections fixed
 */


int main() {
	InitWindow(1920, 1080, "CitySimulation");
	SetTargetFPS(60);

	RoadNetwork network{};
	Vector2 screenPos{};
	float scale{ 1 };

	while (!WindowShouldClose()) {
		BeginDrawing();

		network.designRoads(GetMousePosition());
		network.draw(screenPos, scale);

		ClearBackground(BLACK);
		EndDrawing();
	}

	return 0;
}
