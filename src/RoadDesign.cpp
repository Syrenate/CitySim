#include <raylib.h>
#include "RoadNetwork.h"
#include "MathUtils.h"

Road::Road(RoadType type, bool oneWay) {
	this->type = type;
	this->oneWay = oneWay; }

void RoadNetwork::createRoad(int fromID, int toID, Road road) {
	if (junctions.find(fromID) != junctions.end() && 
	    junctions.find(toID) != junctions.end()) {
		Connection newConnect{ fromID, toID };
		roads[newConnect] = road;
	} }



void RoadNetwork::removeRoad(int fromID, int toID) {
	int roadIndex{};
	for (const auto& [connection, road] : roads) {
		auto [from, to] = connection;
		if (from == fromID && to == toID) {
			roads.erase(connection);
			break; 
		}
	}
}

std::optional<Road> RoadNetwork::getRoad(int fromID, int toID) {
	for (const auto& [connection, road] : roads) {
		auto [from, to] = connection;
		if (from == fromID && to == toID) return road;
	}

	return {};
}

int RoadNetwork::bisectRoad(RoadPoint roadPoint) {
	int newID{ createJunction(roadPoint.second) };

	auto [connection, _] = roadPoint;
	auto [fromID, toID] = connection;
	std::optional<Road> oldRoad{ getRoad(fromID, toID) };

	if (oldRoad) {
		removeRoad(fromID, toID);
		createRoad(fromID, newID, *oldRoad);
		createRoad(newID, toID, *oldRoad);
	}

	return newID;
}

std::optional<RoadPoint> RoadNetwork::getNearbyRoad(Vector2 pos, float searchRadius) {
	for (const auto& [connection, road] : roads) {
		auto [startID, endID] = connection;
		Vector2 startPos{ junctions.at(startID).pos };
		Vector2 endPos{ junctions.at(endID).pos };

		Vector2 intercept{ VecMath::getIncidence(startPos, endPos, pos) };
		float dist{ VecMath::distance(pos, intercept) };

		if (dist <= searchRadius && VecMath::isNearLine(startPos, endPos, pos)) 
			return (RoadPoint){ connection, intercept };
	}

	return {};
}
