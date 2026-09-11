#include <raylib.h>
#include "RoadNetwork.h"
#include "MathUtils.h"

#include <iostream>

void RoadNetwork::designRoads(Vector2 mousePos) {
	if (CheckCollisionPointRec(mousePos, (Rectangle){0,0,worldSize.x,worldSize.y})) {
		targetJunctionID = getNearbyJunction(mousePos);
		targetRoadPoint = getNearbyRoad(mousePos);

		bool hasRoadChanged{};

		if (IsKeyPressed(KEY_S)) isSnappingAngle = !isSnappingAngle;

		if (IsKeyDown(KEY_D)) {
			if (targetJunctionID) removeJunction(*targetJunctionID);
			else if (targetRoadPoint) {
				auto [connection, _] = *targetRoadPoint;
				removeRoad(connection);
			}
			hasRoadChanged = true;
		}

		if (IsKeyPressed(KEY_A)) currentRoadType = ARTERIAL;
		else if (IsKeyPressed(KEY_C)) currentRoadType = COLLECTOR;
		else if (IsKeyPressed(KEY_L)) currentRoadType = LOCAL;


		int LMB{ 0 };
		int RMB{ 1 };

		if (IsMouseButtonPressed(LMB) && !creatingRoad && !hasRoadChanged) {
			creatingRoad = true;
			isValidPlacement = true;

			if (targetJunctionID) {
				roadStart = junctions.at(*targetJunctionID).pos;
				roadStartID = targetJunctionID;
			} else if (targetRoadPoint) {
				roadStartPoint = targetRoadPoint;
				roadStart = targetRoadPoint->second;
			} else {
				roadStart = mousePos; 
			}
		} else if (IsMouseButtonReleased(LMB) && creatingRoad && !hasRoadChanged) {
			try {
				Road currentRoad{ currentRoadType, false };

				Vector2 snappedPos{ (isSnappingAngle) ? VecMath::snapPosition(roadStart, mousePos, ROAD::snappingDivisions) : mousePos };
				Outcome result = createRoad( (Junction){ roadStart, currentJunctionType }, (Junction){ snappedPos, currentJunctionType}, currentRoad);

				if (result == JunctionCollision) {
					std::cout << "failed!";
				}
			} catch (ERROR) {
				std::cout << "super failed!";
			}

			creatingRoad = false;
			hasRoadChanged = true; 

			roadStartID = {};
			roadStart = {};
			roadStartPoint = {};
		}
		
		// if (IsMouseButtonDown(LMB)) hasRoadChanged = true;
		//
		//
		//  DEBUG MOVEMENT - DO NOT INCLUDE IN BUILD
		// if (IsMouseButtonPressed(RMB) && targetJunctionID) 
		// 	junctionMoveID = *targetJunctionID;
		// else if (IsMouseButtonReleased(RMB)) 
		// 	junctionMoveID = 0;
		// if (junctionMoveID) {
		// 	Junction* movingJunction{ &junctions.at(junctionMoveID) };
		// 	movingJunction->pos = mousePos;
		// 	hasRoadChanged = true;
		// }
		//
		if (hasRoadChanged) {
			drawRoadTexture();
			hasRoadChanged = false;
		}

		drawUITexture(mousePos);
		// DrawText(std::to_string(IsRenderTextureValid(roadTexture)).c_str(), 20, 20, 50, RED); 
	}
}

