#include <raylib.h>
// #include <raygui.h>
#include <RoadNetwork.h>
#include "Simulation.h"


int main() {
	Vector2 worldSize{ 3000, 3000 };
	Vector2 screenSize{ 1920, 1080 };

	Simulation simulation{worldSize, screenSize, 60};

	simulation.run();

	return 0;
}
