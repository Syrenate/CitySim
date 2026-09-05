#include <raylib.h>
#include "RoadNetwork.h"
#include "MathUtils.h"

#include <map>
#include <vector>
#include <optional>


Road::Road(RoadType type, bool oneWay) {
	this->type = type;
	this->oneWay = oneWay; }


void RoadNetwork::createRoad(int startID, int endID, Road road) {
	if (junctions.find(startID) == junctions.end() ||
	    junctions.find(endID) == junctions.end()) 
		throw ERROR::DoesNotExist;

	Junction* start{ &junctions.at(startID) };
	Junction* end{ &junctions.at(endID) };

	// Find all interceptions with new road and old ones. 
	typedef std::pair<RoadPoint, float> DistToRoadPoint;
	std::vector<DistToRoadPoint> newRoadPoints{};

	for (const auto& [connection, road] : roads) {
		auto [fromID, toID] = connection;

		Vector2 fromPos{ junctions.at(fromID).pos };
		Vector2 toPos{ junctions.at(toID).pos };

		// float distToStart{ VecMath::getInci	
		
		if (!VecMath::isOnLine(fromPos, toPos, start->pos) && !VecMath::isOnLine(fromPos, toPos, end->pos)){
			std::optional<Vector2> intercept{ VecMath::getIntersection(start->pos, end->pos, fromPos, toPos) };

			if (intercept && VecMath::isInBounds(fromPos, toPos, *intercept)
				      && VecMath::isInBounds(start->pos, end->pos, *intercept)) {

				RoadPoint newPoint{ (Connection){fromID, toID}, *intercept };
				float dist{ VecMath::distance(start->pos, *intercept) };
				newRoadPoints.push_back( (DistToRoadPoint){ newPoint, dist } );
			}
		}
	}

	if (!newRoadPoints.size()) {
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
		removeRoad(startID, endID);

		for (const auto& [roadPoint, dist] : roadPoints) {
			auto [connection, pos] = roadPoint;
			auto [fromID, toID] = connection;

			std::optional<int> newID{ createJunction(pos) };
			Road oldRoad{ getRoad(fromID, toID) };

			removeRoad(fromID, toID);
			if (newID) {
				roads[(Connection){fromID, *newID}] = oldRoad;
				roads[(Connection){*newID, toID}] = oldRoad;

				roads[(Connection){prevSplitID, *newID}] = road;
				// roads[(Connection){prevSplitID,newID}] = road;
				// roads[(Connection){newID,endID}] = road;
				prevSplitID = *newID;
			}

		}

		roads[(Connection){prevSplitID,endID}] = road;
	}
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

	throw ERROR::DoesNotExist;
}

std::optional<int> RoadNetwork::bisectRoad(RoadPoint roadPoint) {
	std::optional<int> newID{ createJunction(roadPoint.second) };

	if (newID) {
		auto [connection, _] = roadPoint;
		auto [fromID, toID] = connection;
		Road oldRoad{ getRoad(fromID, toID) };

		removeRoad(fromID, toID);
		createRoad(fromID, *newID, oldRoad);
		createRoad(*newID, toID, oldRoad);
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

		if (dist <= searchRadius && VecMath::isNearLine(startPos, endPos, pos)) {
			return (RoadPoint){ connection, intercept };
		}
	}

	return {};
}

