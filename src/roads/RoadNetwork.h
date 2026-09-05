#ifndef ROADNETWORK_H
#define ROADNETWORK_H

#include <raylib.h>

#include <utility>
#include <map>
#include <optional>

typedef std::pair<int, int> Connection;
typedef std::pair<Connection, Vector2> RoadPoint;

enum RoadType {
	ARTERIAL,
	COLLECTOR,
	LOCAL };

namespace ROAD {
	const inline int junctionRadius{ 8 };
	const inline int searchDist{ 10 };

	const inline Color junctionColor{ BLUE };
	const inline Color junctionSelectionColor{ RAYWHITE };

	const inline Color roadSelectionColor{ RED };
	const inline Color roadPlanColor = (Color){ 100, 150, 95, 255 };
	const inline std::map<RoadType, Color> roadColors{ {ARTERIAL, (Color){ 255,255,255,255 }},
					      {COLLECTOR, (Color){ 160,160,160,255}},
					      {LOCAL, (Color){100,100,100,255}} };

	const inline std::map<RoadType, float> roadThickness{ {ARTERIAL, 7.0},
					         {COLLECTOR, 5.0},
					         {LOCAL, 3.0} };
}

enum ERROR {
	DoesNotExist, 
	MaxJunctions };

struct Junction {
	Vector2 pos{};
	float radius{};

	Junction(Vector2 pos, float radius);
	Junction() = default;
};

class Road {
public:
	bool oneWay{};
	RoadType type{};

	Road(RoadType type, bool oneWay);
	Road() = default;
};

class RoadNetwork {
public:
	RoadNetwork() = default;

	void designRoads(Vector2 mousePos, Road road);
	void draw(Vector2 screenPos, float scale);
	RoadType currentRoadType{ ARTERIAL };

private:
	std::map<int, Junction> junctions{};
	std::map<Connection, Road> roads{};

	int roadStartID{};
	bool creatingRoad{};

	int junctionMoveID{};
	std::optional<int> targetJunctionID;
	std::optional<RoadPoint> targetRoadPoint;

	RenderTexture2D roadTexture = LoadRenderTexture(1920, 1080);
	RenderTexture2D uiTexture = LoadRenderTexture(1920, 1080);


	int getNewID();
	std::optional<int> createJunction(Vector2 pos);
	void placeJunction(Vector2 mousePos, Road road);
	void removeJunction(int ID);

	Road getRoad(int fromID, int toID);
	void createRoad(int fromID, int toID, Road road);
	std::optional<int> bisectRoad(RoadPoint roadPoint);
	void removeRoad(int fromID, int toID);

	std::optional<int> getNearbyJunction(Vector2 pos, float searchRadius);
	std::optional<RoadPoint> getNearbyRoad(Vector2 pos, float searchRadius);

	void drawRoad(Vector2 startPos, Vector2 endPos, Road road);
	void drawRoadTexture();
	void drawUITexture();
};
#endif
