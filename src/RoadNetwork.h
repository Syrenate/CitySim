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

struct Junction {
	Vector2 pos{};

	Junction(Vector2 pos);
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


	int getNewID();
	int createJunction(Vector2 pos);
	void placeJunction(Vector2 mousePos, Road road);
	void removeJunction(int ID);

	std::optional<Road> getRoad(int fromID, int toID);
	void createRoad(int fromID, int toID, Road road);
	int bisectRoad(RoadPoint roadPoint);
	void removeRoad(int fromID, int toID);

	std::optional<int> getNearbyJunction(Vector2 pos, float searchRadius);
	std::optional<RoadPoint> getNearbyRoad(Vector2 pos, float searchRadius);

};
#endif
