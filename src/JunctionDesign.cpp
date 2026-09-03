#include <raylib.h>
#include "RoadNetwork.h"
#include "MathUtils.h"

#include <vector>

Junction::Junction(Vector2 pos) {
	this->pos = pos; }

int RoadNetwork::getNewID() {
	int maximumID{ (int)1e9 };
	int searchCounter{ 0 };

	while (searchCounter < maximumID) {
		bool isNewID{ true };
		int randomID{ GetRandomValue(1, maximumID) };

		for (const auto& [junctionID, _] : junctions) {
			if (junctionID == randomID) isNewID = false; break;
		}
		
		if (isNewID) return randomID;
	} 

	return 0;
}

int RoadNetwork::createJunction(Vector2 pos) {
	int newID{ getNewID() };

	Junction newJunction{ pos };
	junctions[newID] = newJunction; 

	return newID;	
}	

void RoadNetwork::placeJunction(Vector2 mousePos, Road road) {
	creatingRoad = false;
	bool canPlace{ !targetJunctionID || (targetJunctionID && *targetJunctionID != roadStartID) };

	if (canPlace) {
		if (targetJunctionID) {
			createRoad(roadStartID, *targetJunctionID, road);
		} else if (targetRoadPoint) {
			int newID{ bisectRoad(*targetRoadPoint) };
			createRoad(roadStartID, newID, road);
		} else {
			int newID{ createJunction(mousePos) };
			createRoad(roadStartID, newID, road);
		}
	}

	roadStartID = 0;
}

void RoadNetwork::removeJunction(int ID) {
	std::vector<Connection> connectionsToRemove{};
	for (const auto& [connection, _] : roads) {
		auto [fromID, toID] = connection;
		if (fromID == ID || toID == ID)
			connectionsToRemove.push_back(connection);
	}

	for (auto connection : connectionsToRemove) 
		removeRoad(connection.first, connection.second);

	auto it = junctions.find(ID);
	if (it != junctions.end()) {
		junctions.erase(it);
		targetJunctionID = {};
	}
}




std::optional<int> RoadNetwork::getNearbyJunction(Vector2 pos, float searchRadius) {
	for (const auto& [ID, junction] : junctions) {
		float dist = VecMath::length(VecMath::sub(pos, junction.pos));
		if (dist < searchRadius) return ID; }
	return {};
}
