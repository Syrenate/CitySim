#ifndef SIMULATION_H
#define SIMULATION_H

#include <raylib.h>
#include <RoadNetwork.h>

class Simulation {
public:
	Simulation(Vector2 worldSize, Vector2 screenSize, int targetFPS);

	void run();
	void initRoads();

private:
	RoadNetwork roadNetwork{Vector2{}};
	Rectangle viewport{};
		
	Vector2 worldSize{};
	Rectangle sourceRec{};
	Vector2 minSourceSize{};

	Vector2 screenPos{};
	bool isSprinting{};
	float zoomDirection{};

	int moveStep{ 8 };
	int sprintStep{ 16 };
	Vector2 moveDirection{};

	void zoom();
	float zoomFactor();

	Vector2 screenToWorldPos(Vector2 pos);
	void cameraInputHandling();
	void moveCamera();

	void editRoads();
};

#endif
