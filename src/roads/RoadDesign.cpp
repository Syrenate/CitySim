#include <cmath>
#include <raylib.h>
#include "RoadNetwork.h"
#include "MathUtils.h"

#include <vector>
#include <iostream>

void RoadNetwork::designRoads(Vector2 mousePos, Road road) {
	targetJunctionID = getNearbyJunction(mousePos, ROAD::searchDist);
	targetRoadPoint = getNearbyRoad(mousePos, ROAD::searchDist);

	bool hasRoadChanged{};

	if (IsKeyPressed(KEY_D) && targetJunctionID) {
		removeJunction(*targetJunctionID);
		hasRoadChanged = true;
	}
	
	int LMB{ 0 };
	int RMB{ 1 };
	if (IsMouseButtonPressed(LMB)) {
		creatingRoad = true;

		if (targetJunctionID) 
			roadStartID = *targetJunctionID;
		else {
			std::optional<int> newID{ targetRoadPoint ? bisectRoad(*targetRoadPoint) 
								  : createJunction(mousePos) };
			if (newID) roadStartID = *newID;
			else creatingRoad = false;
		}
		// else if (targetRoadPoint) {
		// 	std::optional<int> newID{ bisectRoad(*targetRoadPoint) };
		//
		// } else {
		// 	std::optional<int> newID{ createJunction(mousePos) };
		//
		// 	if (newID) roadStartID = *newID;
		// 	else creatingRoad = false;
		// }
	} else if (IsMouseButtonReleased(LMB) && creatingRoad) {
		placeJunction(mousePos, (Road){ currentRoadType, false });
		hasRoadChanged = true; 
		creatingRoad = false;
	}
	
	// if (IsMouseButtonDown(LMB)) hasRoadChanged = true;
	//
	//
	//  DEBUG MOVEMENT - DO NOT INCLUDE IN BUILD
	if (IsMouseButtonPressed(RMB) && targetJunctionID) 
		junctionMoveID = *targetJunctionID;
	else if (IsMouseButtonReleased(RMB)) 
		junctionMoveID = 0;
	if (junctionMoveID) {
		Junction* movingJunction{ &junctions.at(junctionMoveID) };
		movingJunction->pos = mousePos;
		hasRoadChanged = true;
	}

	if (hasRoadChanged) {
		drawRoadTexture();
		hasRoadChanged = false;
	}

	drawUITexture();
	// DrawText(std::to_string(IsRenderTextureValid(roadTexture)).c_str(), 20, 20, 50, RED); 
}

