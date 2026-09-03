#ifndef ROADNETWORK_H
#define ROADNETWORK_H

#include <raylib.h>

#include <utility>
#include <map>
#include <optional>

enum RoadType {
	ARTERIAL,
	COLLECTOR,
	LOCAL };


struct Road {
public:
	typedef std::pair<std::pair<int, int>, Vector2> RoadPoint;
	int lanes{ };
	RoadType classification{ };

	Road(int lane_count, RoadType classification); 
	Road() = default;
};

class Intersection {
public:
	int ID{};
	Vector2 pos{};

	typedef std::map<int, Road> RoadMap;
	RoadMap roads{};

	Intersection(int ID, Vector2 pos);
	Intersection() = default;


};

class RoadNetwork {
public:
	RoadNetwork() = default;

	void InputHandler(int laneCount, RoadType roadType);
	void draw(Vector2 screenOrigin);

private:
	typedef std::map<int, Intersection> IntersectionMap;
	IntersectionMap map{ };

	Vector2 roadStart{ };
	int roadStartID{ 0 };
	Intersection* currentTarget{ nullptr };

	std::optional<int> intersectionTargetID;
	std::optional<Road::RoadPoint> roadPoint;

	void evalTargets(Vector2 pos);
	std::optional<int> getNearestIntersection(Vector2 pos, int maxDist);
	std::optional<Road::RoadPoint> getNearestRoadPoint(Vector2 pos, float maxDist);
	void addIntersectionToRoad(std::optional<Road::RoadPoint> roadPoint, int intersectionID);
	void mergeIntersections(int ID1, int ID2);

	std::optional<int> getNewID();
	void addIntersection(int ID, Vector2 pos);
	void removeIntersection(int ID);
	void addRoad(int fromID, int toID, Road road);
	void removeRoad(int fromID, int toID);
};

#endif
