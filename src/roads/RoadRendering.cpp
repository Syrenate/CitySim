#include <raylib.h>
#include "RoadNetwork.h"
#include "MathUtils.h"
#include <string>

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

void RoadNetwork::drawRoad(Connection connection, Road road) {//Vector2 startPos, Vector2 endPos, Road road) {
	auto [fromID, toID] = connection;
	Junction from{ junctions.at(fromID) };
	Junction to{ junctions.at(toID) };
	DrawLineEx(from.pos, to.pos, road.thickness() * 2, road.color());
	// DrawCircleV(from.pos, from.size(), from.color());
	// DrawCircleV(to.pos, to.size(), to.color());//, JUNCTION::junctionColor);
}

void RoadNetwork::drawUITexture(Vector2 mousePos) {
	if (isSnappingAngle) mousePos = VecMath::snapPosition(roadStart, mousePos, ROAD::snappingDivisions);

	BeginTextureMode(uiTexture);
		ClearBackground( (Color){ 0,0,0,0 } );
	
		bool foundNewJunction{};
		if (creatingRoad) {
			if (!roadStartID) DrawCircleV(roadStart, JUNCTION::sizes.at(currentJunctionType), JUNCTION::colors.at(currentJunctionType)); 

			int startID{ roadStartID ? *roadStartID : 0 };
			Junction start{ roadStart, currentJunctionType };

			Vector2 endPos{ targetJunctionID ? junctions.at(*targetJunctionID).pos : 
					targetRoadPoint ? targetRoadPoint->second : mousePos };
			int endID{ targetJunctionID ? *targetJunctionID : 0 };
			Junction end{ endPos, currentJunctionType };

			Road currentRoad{ currentRoadType, false };

			std::optional<RoadPoint> intercept{ getNearestRoadCollision(start, startID, end, endID) };
			std::optional<int> intersectionID{ getJunctionIntersection(currentRoad, start, startID, end, endID) };

			if (intercept) {
				auto [connection, interceptPos] = *intercept;
				auto [fromID, toID] = connection;
				end.pos = interceptPos;
			}
			else if (intersectionID) {
				endID = *intersectionID;
				end.pos = junctions.at(endID).pos;
			} 

			std::optional<int> newTargetJunctionID{ getNearbyJunction(end.pos) };
			if (newTargetJunctionID) {
				foundNewJunction = true;

				endID = *newTargetJunctionID;
				end.pos = junctions.at(endID).pos;

				Vector2 pos{ junctions.at(endID).pos };
				DrawCircleLinesV(pos, JUNCTION::searchDist, JUNCTION::junctionSelectionColor); 
			}

			// targetJunctionID = getNearbyJunction(end.pos);
			// if (targetJunctionID) end.pos = junctions.at(*targetJunctionID).pos;

			bool collidesWithJunction{};
			for (auto& [ID, junction] : junctions) {
				if (ID != startID && ID != endID && CheckCollisionCircleLine(junction.pos, junction.size() + currentRoad.thickness(), start.pos, end.pos)) {
					collidesWithJunction = true;
					break;
				} }
			bool collidesWithRoad{};
			// for (auto& [connection, road] : roads) {
			// 	auto [fromID, toID] = connection;
			// 	if (fromID != startID && fromID != endID && toID != startID && toID != endID && CheckCollisionLines(start.pos, end.pos, junctions.at(fromID).pos, junctions.at(toID).pos, nullptr)) {
			// 		collidesWithRoad = true;
			// 		break;
			// 	}
			// }

			Color roadColor{ collidesWithJunction || collidesWithRoad ? ROAD::invalidColor : currentRoad.color() };


			DrawLineEx(roadStart, end.pos, currentRoad.thickness()*2, roadColor);
			DrawCircleV(end.pos, end.size(), end.color());
			// if (intercept) {
			// 	DrawCircle(100, 100, 100, GREEN);
			// } else {
			// 	if (!targetJunctionID && !targetRoadPoint) {
			// 		DrawCircleV(endPos, JUNCTION::sizes.at(currentJunctionType), JUNCTION::colors.at(currentJunctionType)); }
			// 	DrawLineEx(roadStart, endPos, currentRoad.thickness(), roadColor);
			//
			// }

		}

		if (targetJunctionID && !foundNewJunction) {
			Vector2 pos{ junctions.at(*targetJunctionID).pos };
			DrawCircleLinesV(pos, JUNCTION::searchDist, JUNCTION::junctionSelectionColor); 
		} else if (targetRoadPoint && !foundNewJunction) {
			DrawCircle(10,10,10,GREEN);
			auto [_, pos] = *targetRoadPoint;
			DrawCircleV(pos, JUNCTION::sizes.at(currentJunctionType), ROAD::roadSelectionColor);
		}

		DrawText(std::to_string(junctions.size()).c_str(), 30, 30, 30, RED);
		DrawText(std::to_string(roads.size()).c_str(), 30, 60, 30, RED);

		int count{};
		for (const auto& [ID, junction] : junctions) {
			DrawText(std::to_string(junction.pos.x).c_str(), 100, 100+count*20, 20, RED);
			DrawText(std::to_string(junction.pos.y).c_str(), 350, 100+count*20, 20, RED);
			count++;

		}

	EndTextureMode();
}

void RoadNetwork::drawRoadTexture() {
	BeginTextureMode(roadTexture);

		ClearBackground( (Color){ 0,0,0,0 } );
		 
		for (auto& [connection, road] : roads) {
			drawRoad(connection, road);
		}
		for (auto& [ID, junction] : junctions) {
			DrawCircleV(junction.pos, junction.size(), junction.color());
		}

	EndTextureMode();
}
