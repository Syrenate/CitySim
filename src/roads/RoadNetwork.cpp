#include <algorithm>
#include <raylib.h>
#include "RoadNetwork.h"
#include "MathUtils.h"

#include <iostream>
#include <map>
#include <vector>
#include <optional>
#include <cmath>

Road::Road(RoadType type, bool oneWay) {
	this->type = type;
	this->oneWay = oneWay; }

RoadNetwork::RoadNetwork(Vector2 worldSize) {
	roadTexture = LoadRenderTexture(round(worldSize.x), round(worldSize.y));
	uiTexture = LoadRenderTexture(round(worldSize.x), round(worldSize.y));
	this->worldSize = worldSize;
}

std::optional<int> RoadNetwork::getJunctionIntersection(Road road, Junction start, int startID, Junction end, int endID) {
	typedef std::pair<int, float> JunctionDistance;
	std::vector<JunctionDistance> junctionTracker{};

	float minDistance{ 99999999.0 };
	for (auto& [ID, junction] : junctions) {
		bool isStart{ (roadStartID && ID != *roadStartID) || !roadStartID };
		if (ID != startID && ID != endID) {
			Vector2 incidence{ VecMath::getIncidence(start.pos, end.pos, junction.pos) };

			if (VecMath::isNearLine(start.pos, end.pos, incidence)) {
				float distanceToRoad{ VecMath::distance(junction.pos, incidence) };

				if (distanceToRoad < junction.size() + road.thickness()) {
					float distToStart{ VecMath::distance(incidence, start.pos) };

					junctionTracker.push_back( (JunctionDistance){ ID, distToStart } ); 
					minDistance = std::min(distToStart, minDistance);
				}
			}
		}
	}

	for (auto& [ID, dist] : junctionTracker) 
		if (dist == minDistance) return ID;
	 
	return {};
}

std::optional<RoadPoint> RoadNetwork::getRoadCollision(Junction start, Junction end, Connection connection, Road road) {
	auto [fromID, toID] = connection;

	Vector2 fromPos{ junctions.at(fromID).pos };
	Vector2 toPos{ junctions.at(toID).pos };
	
	// bool isParallel{ VecMath::getGradient(start.pos, end.pos) == VecMath::getGradient(fromPos, toPos) };
	// if (isParallel && VecMath::isOnLine(start.pos, end.pos, fromPos)) throw ERROR::SameRoad;

	// if (VecMath::distance(start.pos, VecMath::getIncidence(fromPos, toPos, start.pos)) >= road.thickness() + start.size() &&
	//     VecMath::distance(end.pos, VecMath::getIncidence(fromPos, toPos, end.pos)) >= road.thickness() + end.size()) {
	std::optional<Vector2> intercept{ VecMath::getIntersection(start.pos, end.pos, fromPos, toPos) };

	if (intercept && VecMath::isInBounds(fromPos, toPos, *intercept)
		      && VecMath::isInBounds(start.pos, end.pos, *intercept)) {
		RoadPoint newPoint{ (Connection){fromID, toID}, *intercept };
		return newPoint;
	}
	
	return {};

}


std::optional<RoadPoint> RoadNetwork::getNearestRoadCollision(Junction start, int startID, Junction end, int endID) {
	typedef std::pair<RoadPoint, float> DistToRoadPoint;
	std::vector<DistToRoadPoint> newRoadPoints{};

	float minDistance{ 9999999999.0 };
	for (auto& [connection, road] : roads) {
		auto [fromID, toID] = connection;
		if (startID == fromID || startID == toID || endID == fromID || endID == toID) continue;

		std::optional<RoadPoint> collision{ getRoadCollision(start, end, connection, road) };
		if (collision) {
			bool farEnoughFromStart{ VecMath::distance(collision->second, start.pos) >= start.size() };
			if (farEnoughFromStart) {
				float distance{ VecMath::distance(start.pos, collision->second) };
				newRoadPoints.push_back( (DistToRoadPoint){ *collision, distance } );

				minDistance = std::min(distance, minDistance);
			}
		} 
	}

	for (const auto& [roadPoint, dist] : newRoadPoints) {
		if (dist == minDistance) return roadPoint; }

	return {};
}


Outcome RoadNetwork::createRoad(Junction start, Junction end, Road road) {
	if (VecMath::equal(start.pos, end.pos)) return Outcome::InvalidRoad;

	bool startIsEnd{ roadStartID && targetJunctionID && *roadStartID == *targetJunctionID };
	bool junctionsTooClose{ VecMath::distance(start.pos, end.pos) < start.size() + end.size() };
	if (startIsEnd || junctionsTooClose) return Outcome::InvalidRoad;


	int startID{ getNewID() };
	if (roadStartID) startID = *roadStartID;
	else if (roadStartPoint) startID = bisectRoad(*roadStartPoint);
	else junctions[startID] = start;

	int endID{ targetJunctionID ? *targetJunctionID : getNewID() };
	end.pos = targetJunctionID ? junctions.at(*targetJunctionID).pos : 
		 targetRoadPoint ? targetRoadPoint->second : end.pos;

	std::optional<int> intersectionID{ getJunctionIntersection(road, start, startID, end, endID) };
	std::optional<RoadPoint> collision{ getNearestRoadCollision(start, startID, end, endID) };

	if (collision) end.pos = collision->second;
	else if (intersectionID) end.pos = junctions.at(*intersectionID).pos; 
	

	std::optional<int> newTargetJunctionID = getNearbyJunction(end.pos);
	bool foundNewJunction{};
	if (newTargetJunctionID) {
		foundNewJunction = true;
		endID = *newTargetJunctionID;
		end.pos = junctions.at(endID).pos;
	}

	for (auto& [ID, junction] : junctions) {
		if (ID != startID && ID != endID && CheckCollisionCircleLine(junction.pos, junction.size() + road.thickness(), start.pos, end.pos)) {
			return Outcome::InvalidRoad;
		} }


	if (!foundNewJunction) {
		if (collision) endID = bisectRoad(*collision);
		else if (newTargetJunctionID) endID = *newTargetJunctionID;
		else if (targetRoadPoint) endID = bisectRoad(*targetRoadPoint);
		else junctions[endID] = end;
	}
	

	Connection newConnect{ startID, endID };
	roads[newConnect] = road;

	return Outcome::Success;
}


void RoadNetwork::removeRoad(Connection connection) {
	auto [fromID, toID] = connection;
	int roadIndex{};
	for (const auto& [connection, road] : roads) {
		auto [from, to] = connection;
		if (from == fromID && to == toID) {
			roads.erase(connection);
			break; 
		}
	}
}

Road RoadNetwork::getRoad(int fromID, int toID) {
	for (const auto& [connection, road] : roads) {
		auto [from, to] = connection;
		if (from == fromID && to == toID) return road;
	}

	std::cout<<"\nNo road!\n";
	throw ERROR::DoesNotExist;
}

int RoadNetwork::bisectRoad(RoadPoint roadPoint) {
	auto [connection, pos] = roadPoint;
	int newID{ placeJunction( (Junction){ pos, JUNCTION::defaultJunction } ) };
	// std::optional<int> newID{ createJunction(roadPoint.second) };

	// auto [connection, road] = roadPoint;
	auto [fromID, toID] = connection;
	Road oldRoad{ getRoad(fromID, toID) };

	removeRoad(connection);
	roads[(Connection){fromID,newID}] = oldRoad;
	roads[(Connection){newID,toID}] = oldRoad;

	return newID;
}

std::optional<RoadPoint> RoadNetwork::getNearbyRoad(Vector2 pos) {
	for (auto& [connection, road] : roads) {
		auto [startID, endID] = connection;
		Vector2 startPos{ junctions.at(startID).pos };
		Vector2 endPos{ junctions.at(endID).pos };

		Vector2 intercept{ VecMath::getIncidence(startPos, endPos, pos) };
		float dist{ VecMath::distance(pos, intercept) };

		if (dist <= road.thickness() + ROAD::searchDist && VecMath::isNearLine(startPos, endPos, pos)) {
			return (RoadPoint){ connection, intercept };
		}
	}

	return {};
}

