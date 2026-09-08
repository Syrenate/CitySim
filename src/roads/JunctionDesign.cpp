#include <raylib.h>
#include "RoadNetwork.h"
#include "MathUtils.h"

#include <vector>

Junction::Junction(Vector2 pos, JunctionType type) {
	this->pos = pos; 
	this->type = type; }

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
		searchCounter++;
	} 

	throw ERROR::MaxJunctions; }

int RoadNetwork::placeJunction(Junction junction) {
	int ID{ getNewID() };
	junctions[ID] = junction; 
	return ID; }

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
		targetRoadPoint = {};
		junctionMoveID = 0;
	}
}


std::optional<int> RoadNetwork::getNearbyJunction(Vector2 pos) {
	for (auto& [ID, junction] : junctions) {
		float dist = VecMath::length(VecMath::sub(pos, junction.pos));
		if (dist < JUNCTION::searchDist + junction.size()) return ID; }
	return {};
}
