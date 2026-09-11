#include <raylib.h>
#include "RoadNetwork.h"
#include "MathUtils.h"

#include <iostream>
#include <map>
#include <vector>
#include <optional>

Road::Road(RoadType type, bool oneWay) {
	this->type = type;
	this->oneWay = oneWay; }


bool RoadNetwork::doesRoadIntersectJunction(Road road, Junction start, int startID, Junction end, int endID) {
	for (auto& [ID, junction] : junctions) {
		bool isStart{ (roadStartID && ID != *roadStartID) || !roadStartID };
		if (ID != startID && ID != endID) {
			Vector2 incidence{ VecMath::getIncidence(start.pos, end.pos, junction.pos) };

			if (VecMath::isNearLine(start.pos, end.pos, incidence)) {
				float distanceToRoad{ VecMath::distance(junction.pos, incidence) };
				if (distanceToRoad < junction.size() + std::max(start.size(), end.size())) return true;//Outcome::JunctionCollision;
			}
		}
	}
	 
	return false;
}

Outcome RoadNetwork::createRoad(Junction start, Junction end, Road road) {
	// if (junctions.find(startID) == junctions.end() ||
	//     junctions.find(endID) == junctions.end()) 
	// 	throw ERROR::DoesNotExist;
	//
	// Junction* start{ &junctions.at(startID) };
	// Junction* end{ &junctions.at(endID) };
	
	// Check the road does not collide with another intersection
	// bool canCreate{ true };
	// for (auto& [ID, junction] : junctions) {
	// 	bool endPointsValid{ (startsAtJunction || VecMath::distance(start.pos, junction.pos) >= start.size() + junction.size()) &&
	// 	    		      (VecMath::distance(end.pos, junction.pos) >= end.size() + junction.size()) };
	// 	bool roadValid{ VecMath::distance(junction.pos, VecMath::getIncidence(start.pos, end.pos, junction.pos)) >= junction.size() + ROAD::roadThickness.at(road.type)/2 };
	//
	// 	if (false) {//!endPointsValid || !roadValid) {
	// 		canCreate = false;
	// 		break;
	// 	}
	// }
	bool startsAtJunction { roadStartID ? true : false };//{ junction.pos.x == start.pos.x && junction.pos.y == start.pos.y };
	bool startsAtRoad{ roadStartPoint ? true : false };

	int startID{ startsAtJunction ? *roadStartID : getNewID() };
	int endID{ targetJunctionID ? *targetJunctionID : getNewID() };

	bool doesIntersectJunction{ doesRoadIntersectJunction(road, start, startID, end, endID) };
	if (doesIntersectJunction) return Outcome::JunctionCollision;

	if (!startsAtJunction && startsAtRoad) {
		startID = bisectRoad(*roadStartPoint);
	} else if (!startsAtJunction) {
		junctions[startID] = start; }


	// Find all interceptions with new road and old ones. 
	typedef std::pair<RoadPoint, float> DistToRoadPoint;
	std::vector<DistToRoadPoint> newRoadPoints{};

	for (auto& [connection, road] : roads) {
		auto [fromID, toID] = connection;

		Vector2 fromPos{ junctions.at(fromID).pos };
		Vector2 toPos{ junctions.at(toID).pos };

		// float distToStart{ VecMath::getInci	
		

		if (VecMath::distance(start.pos, VecMath::getIncidence(fromPos, toPos, start.pos)) >= road.thickness() + start.size() &&
		    VecMath::distance(end.pos, VecMath::getIncidence(fromPos, toPos, end.pos)) >= road.thickness() + end.size()) {
		// if (!VecMath::isOnLine(fromPos, toPos, start.pos) && !VecMath::isOnLine(fromPos, toPos, end.pos)){
			std::optional<Vector2> intercept{ VecMath::getIntersection(start.pos, end.pos, fromPos, toPos) };

			if (intercept && VecMath::isInBounds(fromPos, toPos, *intercept)
				      && VecMath::isInBounds(start.pos, end.pos, *intercept)) {

				RoadPoint newPoint{ (Connection){fromID, toID}, *intercept };
				float dist{ VecMath::distance(start.pos, *intercept) };
				newRoadPoints.push_back( (DistToRoadPoint){ newPoint, dist } );
			}
		}
	}

	if (!newRoadPoints.size()) {
		if (!targetJunctionID && targetRoadPoint) {
			endID = bisectRoad(*targetRoadPoint);
		} else if (!targetJunctionID) {
			junctions[endID] = end; }

		Connection newConnect{ startID, endID };
		roads[newConnect] = road;
	} else {
		// Sort road interceptions by distance to start of new road.
		std::vector<DistToRoadPoint> roadPoints{};
		for (const auto& tracker : newRoadPoints) {
			const auto [roadPoint, dist] = tracker;

			// // Check this point is far enough from the road start/end.
			// auto [connection, pos] = roadPoint;
			// auto [fromID, toID] = connection;
			// if (VecMath::distance(pos, junctions.at(fromID).pos) >  
			//

			int insertionIndex{};
			for (const auto& [otherRoadPoint, otherDist] : roadPoints) {
				if (otherDist > dist) break;
				insertionIndex++;
			}

			roadPoints.insert(roadPoints.begin() + insertionIndex, tracker);
		}

		// Bisect each road (and build new road) sequentially.
		int prevSplitID{ startID };
		float prevDist{ 0 };

		bool canFinishRoad{ true };
		for (const auto& [roadPoint, dist] : roadPoints) {
			auto [connection, pos] = roadPoint;
			auto [fromID, toID] = connection;
			
			bool intersectsJunction{ dist - prevDist < JUNCTION::sizes.at(JUNCTION::defaultJunction) + junctions.at(prevSplitID).size() };
			bool intersectsRoad{};
			// for (const auto& [subConnection subPos] : roads) {


			if (!intersectsJunction) {
				int newID{ placeJunction( (Junction){ pos, JUNCTION::defaultJunction } ) };
				std::cout << '\n' << fromID << ',' << toID << '\n';
				Road oldRoad{ getRoad(fromID, toID) };

				removeRoad(fromID, toID);
				roads[(Connection){fromID, newID}] = oldRoad;
				roads[(Connection){newID, toID}] = oldRoad;

				roads[(Connection){prevSplitID, newID}] = road;
				prevSplitID = newID;
				prevDist = dist;
			} else {
				canFinishRoad = false;
				break;
			}
			// roads[(Connection){prevSplitID,newID}] = road;
			// roads[(Connection){newID,endID}] = road;
		}

		if (canFinishRoad) {
			if (!targetJunctionID && targetRoadPoint) {
				endID = bisectRoad(*targetRoadPoint);
			} else if (!targetJunctionID) {
				junctions[endID] = end; }

			roads[(Connection){prevSplitID,endID}] = road;
		} 
	}

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

