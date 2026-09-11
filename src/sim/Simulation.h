#ifndef SIMULATION_H
#define SIMULATION_H

#include <raylib.h>
#include <RoadNetwork.h>

class ViewportCamera {
public:
	ViewportCamera(Rectangle viewport, Vector2 worldSize);
	ViewportCamera() = default;

	Rectangle viewport{};

	void move();
	void inputHandling();
	void drawRoads(RoadNetwork roadNetwork);
	Vector2 screenToWorldPos(Vector2 pos);

private:
	Rectangle sourceRec{};

	Vector2 worldSize{};
	Vector2 minSourceSize{};

	Vector2 screenPos{};
	bool isSprinting{};

	float zoomDirection{};
	float zoomSpeed{ 10.0f };

	int moveSpeed{ 8 };
	float sprintFactor{ 2 };
	Vector2 moveDirection{};

	void zoom();
	float zoomFactor();
};

class Simulation {
public:
	Simulation(Vector2 worldSize, Vector2 screenSize, int targetFPS);

	void run();

private:
	RoadNetwork roadNetwork{Vector2{}};
	ViewportCamera camera{};

	void initRoads();
	void editRoads();
};

#endif
