#include <raylib.h>
#include "RoadNetwork.h"
#include "MathUtils.h"

void drawLineScaled(Vector2 start, Vector2 end, float thickness, Color color, Vector2 offset, float scale) {
	Vector2 newStart{ start };
	Vector2 newEnd{ end };
	DrawLineEx(newStart, newEnd, thickness, color); }

void drawCircleScaled(Vector2 pos, float radius, Color color, Vector2 offset, float scale) {
	Vector2 newPos{ pos };
	DrawCircleV(newPos, radius, color); }

void RoadNetwork::draw(Vector2 screenPos, float scale) {
	Rectangle srcRec{ 0.0, 0.0, (float)roadTexture.texture.width, -(float)roadTexture.texture.height };
	DrawTextureRec(roadTexture.texture, srcRec, Vector2{0,0}, RAYWHITE);
	DrawTextureRec(uiTexture.texture, srcRec, Vector2{0,0}, RAYWHITE);
	DrawFPS(1900, 20);
}

void RoadNetwork::drawRoad(Vector2 startPos, Vector2 endPos, Road road) {
	DrawLineEx(startPos, endPos, ROAD::roadThickness.at(road.type), 
			             ROAD::roadColors.at(road.type) );
	DrawCircleV(startPos, ROAD::junctionRadius, ROAD::junctionColor);
	DrawCircleV(endPos, ROAD::junctionRadius, ROAD::junctionColor);
}

void RoadNetwork::drawUITexture() {
	BeginTextureMode(uiTexture);
		ClearBackground( (Color){ 0,0,0,0 } );

		if (targetJunctionID) {
			Vector2 pos{ junctions.at(*targetJunctionID).pos };
			DrawCircleLinesV(pos, ROAD::searchDist, ROAD::junctionSelectionColor);
		} else if (targetRoadPoint) {
			auto [_, pos] = *targetRoadPoint;
			DrawCircleV(pos, ROAD::junctionRadius, ROAD::roadSelectionColor);
		}
	
		if (roadStartID) {
			Vector2 startPos{ junctions.at(roadStartID).pos };
			Vector2 endPos{ targetJunctionID ? junctions.at(*targetJunctionID).pos : 
				targetRoadPoint ? targetRoadPoint->second : GetMousePosition() };

			drawRoad(startPos, endPos, (Road){ currentRoadType, false }); 

			for (const auto& [connection, road] : roads) {
				auto [fromID, toID] = connection;

				Vector2 fromPos{ junctions.at(fromID).pos };
				Vector2 toPos{ junctions.at(toID).pos };

				// DrawCircleV(startPos, 20, GREEN);
				// DrawCircleV(endPos, 20, GREEN);
				if (!VecMath::isOnLine(fromPos, toPos, startPos) && !VecMath::isOnLine(fromPos, toPos, endPos)){
					std::optional<Vector2> intercept{ VecMath::getIntersection(startPos, endPos, fromPos, toPos) };

					if (intercept && VecMath::isInBounds(fromPos, toPos, *intercept)
						      && VecMath::isInBounds(startPos, endPos, *intercept)) {
						DrawCircleV(*intercept, ROAD::junctionRadius, ROAD::roadSelectionColor);
					}
				}
			}
		}

	EndTextureMode();
}

void RoadNetwork::drawRoadTexture() {
	BeginTextureMode(roadTexture);
		ClearBackground( (Color){ 0,0,0,0 } );

		// Draw all roads
		for (const auto& [connection, road] : roads) {
			Color roadColor{ ROAD::roadColors.at(road.type) };
			float roadThickness{ ROAD::roadThickness.at(road.type) };

			auto [fromID, toID] = connection;
			DrawLineEx(junctions.at(fromID).pos, junctions.at(toID).pos, roadThickness, roadColor); }

		// Draw road in progress

		for (const auto& [_, junction] : junctions) {
			DrawCircleV(junction.pos, ROAD::junctionRadius, ROAD::junctionColor); }

		// if (targetJunctionID) {
		// 	Vector2 junctionPos{ junctions.at(*targetJunctionID).pos };
		// 	DrawCircleLinesV(junctionPos, ROAD::searchDist, ROAD::junctionSelectionColor); }
		//
		// else if (targetRoadPoint) {
		// 	Vector2 roadPos{ targetRoadPoint->second };
		// 	DrawCircleV(roadPos, ROAD::junctionRadius, ROAD::roadSelectionColor); }

	EndTextureMode();
}
