#include <raylib.h>
#include "RoadNetwork.h"
#include "MathUtils.h"

#include <numeric>
#include <iostream>
#include <map>
#include <vector>
#include <optional>

Road::Road(RoadType type, bool oneWay) {
	this->type = type;
	this->oneWay = oneWay; }


std::optional<int> RoadNetwork::getJunctionIntersection(Road road, Junction start, int startID, Junction end, int endID) {
	for (auto& [ID, junction] : junctions) {
		bool isStart{ (roadStartID && ID != *roadStartID) || !roadStartID };
		if (ID != startID && ID != endID) {
			Vector2 incidence{ VecMath::getIncidence(start.pos, end.pos, junction.pos) };

			if (VecMath::isNearLine(start.pos, end.pos, incidence)) {
				float distanceToRoad{ VecMath::distance(junction.pos, incidence) };
				if (distanceToRoad < junction.size() + std::max(start.size(), end.size())) return ID;

			}
		}
	}
	 
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


	int startID{ roadStartID ? *roadStartID : getNewID() };
	int endID{ targetJunctionID ? *targetJunctionID : getNewID() };



	if (roadStartID) startID = *roadStartID;
	else if (roadStartPoint) startID = bisectRoad(*roadStartPoint);
	else junctions[startID] = start;


	std::optional<RoadPoint> collision{ getNearestRoadCollision(start, startID, end, endID) };
	std::optional<int> intersectionID{ getJunctionIntersection(road, start, startID, end, endID) };

	if (intersectionID) end.pos = junctions.at(*intersectionID).pos; 
	else if (collision) {
		end.pos = collision->second;
	}

	targetJunctionID = getNearbyJunction(end.pos);
	if (targetJunctionID) end.pos = junctions.at(*targetJunctionID).pos;

	if (targetJunctionID) endID = *targetJunctionID;
	else if (collision) endID = bisectRoad(*collision);
	else if (targetRoadPoint) endID = bisectRoad(*targetRoadPoint);
	else junctions[endID] = end;
	

	Connection newConnect{ startID, endID };
	roads[newConnect] = road;

	return Outcome::Success;
}


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

	removeRoad(fromID, toID);
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

