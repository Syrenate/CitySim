#include <raylib.h>
#include <raygui.h>

#include "RoadNetwork.h"
#include "MathUtils.h"


int main() {
	RoadNetwork network{};

	InitWindow(1920, 1080, "CitySimulation");
	RoadType roadType{ ARTERIAL };
	int laneCount{1};
	Vector2 screenOrigin{};
	Vector2 prevMousePos{ GetMousePosition() };

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);

		if (IsKeyPressed(KEY_A)) roadType = ARTERIAL;
		if (IsKeyPressed(KEY_C)) roadType = COLLECTOR;
		if (IsKeyPressed(KEY_L)) roadType = LOCAL;

		if (IsMouseButtonDown(0) && IsKeyDown(KEY_LEFT_ALT)) {
			// screenOrigin = VecMath::add(screenOrigin, VecMath::sub(GetMousePosition(), prevMousePos));
		} else {
			network.InputHandler(laneCount, roadType);
		}


		network.draw(screenOrigin);

		EndDrawing();

		prevMousePos = GetMousePosition();
	}

	return 0;
}
