#include <raylib.h>
#include "RoadNetwork.h"

#include <map>
#include <optional>

namespace ROAD {
	int junctionRadius{ 8 };
	int searchDist{ 10 };

	Color junctionColor{ BLUE };
	Color junctionSelectionColor{ RAYWHITE };

	Color roadSelectionColor{ RED };
	Color roadPlanColor = (Color){ 100, 150, 95, 255 };
	std::map<RoadType, Color> roadColors{ {ARTERIAL, (Color){ 255,255,255,255 }},
					      {COLLECTOR, (Color){ 160,160,160,255}},
					      {LOCAL, (Color){100,100,100,255}} };

	std::map<RoadType, float> roadThickness{ {ARTERIAL, 7.0},
					         {COLLECTOR, 5.0},
					         {LOCAL, 3.0} };
}


void RoadNetwork::designRoads(Vector2 mousePos, Road road) {
	targetJunctionID = getNearbyJunction(mousePos, ROAD::searchDist);
	targetRoadPoint = getNearbyRoad(mousePos, ROAD::searchDist);

	if (IsKeyPressed(KEY_D) && targetJunctionID) 
		removeJunction(*targetJunctionID);
	
	int LMB{ 0 };
	int RMB{ 1 };

	if (IsMouseButtonPressed(LMB)) {
		creatingRoad = true;

		if (targetJunctionID) 
			roadStartID = *targetJunctionID;
		else if (targetRoadPoint) {
			int newID{ bisectRoad(*targetRoadPoint) };
			roadStartID = newID;
		} else {
			int newID{ createJunction(mousePos) };
			roadStartID = newID;
		}
	} else if (IsMouseButtonReleased(LMB))
		placeJunction(mousePos, (Road){ currentRoadType, false });
	

	if (IsMouseButtonPressed(RMB) && targetJunctionID) 
		junctionMoveID = *targetJunctionID;
	else if (IsMouseButtonReleased(RMB)) 
		junctionMoveID = 0;
	

	if (junctionMoveID) {
		Junction* movingJunction{ &junctions.at(junctionMoveID) };
		movingJunction->pos = mousePos;
	}
}

void drawLineScaled(Vector2 start, Vector2 end, float thickness, Color color, Vector2 offset, float scale) {
	Vector2 newStart{ start };
	Vector2 newEnd{ end };
	DrawLineEx(newStart, newEnd, thickness, color); }

void drawCircleScaled(Vector2 pos, float radius, Color color, Vector2 offset, float scale) {
	Vector2 newPos{ pos };
	DrawCircleV(newPos, radius, color); }

void RoadNetwork::draw(Vector2 screenPos, float scale) {
	for (const auto& [connection, road] : roads) {
		Color roadColor{ ROAD::roadColors.at(road.type) };
		float roadThickness{ ROAD::roadThickness.at(road.type) };

		auto [fromID, toID] = connection;

		DrawLineEx(junctions.at(fromID).pos, junctions.at(toID).pos, roadThickness, roadColor); }

	if (roadStartID) {
		Vector2 startPos{ junctions.at(roadStartID).pos };
		Vector2 endPos{ targetJunctionID ? junctions.at(*targetJunctionID).pos : 
			targetRoadPoint ? targetRoadPoint->second : GetMousePosition() };

		DrawCircleV(endPos, ROAD::junctionRadius, ROAD::junctionColor);
		DrawLineEx(startPos, endPos, ROAD::roadThickness.at(currentRoadType), 
				ROAD::roadColors.at(currentRoadType) );
	}

	for (const auto& [_, junction] : junctions) {
		DrawCircleV(junction.pos, ROAD::junctionRadius, ROAD::junctionColor); }

	if (targetJunctionID) {
		Vector2 junctionPos{ junctions.at(*targetJunctionID).pos };
		DrawCircleLinesV(junctionPos, ROAD::searchDist, ROAD::junctionSelectionColor); }

	else if (targetRoadPoint) {
		Vector2 roadPos{ targetRoadPoint->second };
		DrawCircleV(roadPos, ROAD::junctionRadius, ROAD::roadSelectionColor); }

}
