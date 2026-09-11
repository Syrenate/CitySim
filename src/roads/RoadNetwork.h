#ifndef ROADNETWORK_H
#define ROADNETWORK_H

#include <raylib.h>

#include <utility>
#include <map>
#include <optional>

enum ERROR {
	DoesNotExist, 
	MaxJunctions, 
	SameRoad };

enum Outcome {
	Success,
	JunctionCollision,
	InvalidRoad };


enum RoadType {
	ARTERIAL,
	COLLECTOR,
	LOCAL };

enum JunctionType {
	ROUNDABOUT };

typedef std::pair<int, int> Connection;
typedef std::pair<Connection, Vector2> RoadPoint;

namespace ROAD {
	const inline float searchDist{ 10 };
	const inline Color roadSelectionColor{ RED };
	const inline Color roadPlanColor = (Color){ 100, 150, 95, 255 };
	const inline std::map<RoadType, Color> roadColors{ {ARTERIAL, (Color){ 255,255,255,255 }},
					      {COLLECTOR, (Color){ 160,160,160,255}},
					      {LOCAL, (Color){100,100,100,255}} };
	const inline std::map<RoadType, float> roadThickness{ {ARTERIAL, 3.5},
					         {COLLECTOR, 2.5},
					         {LOCAL, 1.5} };
	const inline Color invalidColor{ RED };
	const inline int snappingDivisions{ 32 };
}


namespace JUNCTION {
	const inline int junctionRadius{ 8 };
	const inline int searchDist{ 10 };

	const inline Color junctionColor{ BLUE };
	const inline Color junctionSelectionColor{ RAYWHITE };

	const inline JunctionType defaultJunction{ ROUNDABOUT };
	const inline std::map<JunctionType, float> sizes{ {ROUNDABOUT, 10} };
	const inline std::map<JunctionType, Color> colors{ {ROUNDABOUT, BLUE} };
}



struct Junction {
	Vector2 pos{};
	JunctionType type{};

	Junction(Vector2 pos, JunctionType type);
	Junction() = default;

	float size() { return JUNCTION::sizes.at(type); }
	Color color() { return JUNCTION::colors.at(type); }
};

class Road {
public:
	bool oneWay{};
	RoadType type{};

	Road(RoadType type, bool oneWay);
	Road() = default;

	float thickness() { return ROAD::roadThickness.at(type); }
	Color color() { return ROAD::roadColors.at(type); }
};

class RoadNetwork {
public:
	RoadNetwork(Vector2 worldSize);

	void designRoads(Vector2 mousePos);
	void draw(Rectangle srcRec, Rectangle viewport);

	RoadType currentRoadType{ ARTERIAL };
	JunctionType currentJunctionType{ ROUNDABOUT };

	RenderTexture2D roadTexture;
	RenderTexture2D uiTexture;

private:
	Vector2 worldSize;
	std::map<int, Junction> junctions{};
	std::map<Connection, Road> roads{};

	bool isValidPlacement{ true };
	Vector2 roadStart{};
	std::optional<int> roadStartID{};
	std::optional<RoadPoint> roadStartPoint{};
	bool creatingRoad{};
	bool isSnappingAngle{};

	int junctionMoveID{};
	std::optional<int> targetJunctionID;
	std::optional<RoadPoint> targetRoadPoint;


	int getNewID();
	std::optional<int> createJunction(Vector2 pos);
	int placeJunction(Junction junction);
	void removeJunction(int ID);

	std::optional<int> getJunctionIntersection(Road road, Junction start, int startID, Junction end, int endID);
	Road getRoad(int fromID, int toID);
	Outcome createRoad(Junction start, Junction end, Road road);
	int bisectRoad(RoadPoint roadPoint);
	void removeRoad(Connection connection);

	std::optional<RoadPoint> getRoadCollision(Junction start, Junction end, Connection connection, Road road);
	std::optional<RoadPoint> getNearestRoadCollision(Junction start, int startID, Junction end, int endID);

	std::optional<int> getNearbyJunction(Vector2 pos);
	std::optional<RoadPoint> getNearbyRoad(Vector2 pos);

	void drawRoad(Connection connection, Road road);
	void drawRoadTexture();
	void drawUITexture(Vector2 mousePos);
};
#endif
