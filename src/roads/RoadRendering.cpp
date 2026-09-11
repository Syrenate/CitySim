#include <raylib.h>
#include "RoadNetwork.h"
#include "MathUtils.h"
#include <string>

void RoadNetwork::draw(Rectangle srcRec, Rectangle viewport) {
	float width = viewport.width;
	float height = viewport.height;

	srcRec.y = -srcRec.y - srcRec.height;
	srcRec.height *= -1;

	Vector2 pos{ viewport.x, viewport.y };

	DrawTexturePro(roadTexture.texture, srcRec, viewport, Vector2{0.0f, 0.0f}, 0.0f, RAYWHITE);
	DrawTexturePro(uiTexture.texture, srcRec, viewport, Vector2{0.0f, 0.0f}, 0.0f, RAYWHITE);

	DrawRectangleLinesEx(viewport, 5, RAYWHITE);

	DrawFPS(pos.x + 10, pos.y + 10);
}

void RoadNetwork::drawRoad(Connection connection, Road road) {//Vector2 startPos, Vector2 endPos, Road road) {
	auto [fromID, toID] = connection;
	Junction from{ junctions.at(fromID) };
	Junction to{ junctions.at(toID) };
	DrawLineEx(from.pos, to.pos, road.thickness() * 2, road.color());
}

void RoadNetwork::drawUITexture(Vector2 mousePos) {
	if (isSnappingAngle) mousePos = VecMath::snapPosition(roadStart, mousePos, ROAD::snappingDivisions);

	BeginTextureMode(uiTexture);

	ClearBackground( (Color){ 0,0,0,0 } );
	DrawRectangleLinesEx((Rectangle){ 0, 0, worldSize.x, worldSize.y}, 20, GRAY);

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


		bool collidesWithJunction{};
		for (auto& [ID, junction] : junctions) {
			if (ID != startID && ID != endID && CheckCollisionCircleLine(junction.pos, junction.size() + currentRoad.thickness(), start.pos, end.pos)) {
				collidesWithJunction = true;
				break;
			} }
		bool collidesWithRoad{};

		Color roadColor{ collidesWithJunction || collidesWithRoad ? ROAD::invalidColor : currentRoad.color() };


		DrawLineEx(roadStart, end.pos, currentRoad.thickness()*2, roadColor);
		DrawCircleV(end.pos, end.size(), end.color());
	}

	if (targetJunctionID && !foundNewJunction) {
		Vector2 pos{ junctions.at(*targetJunctionID).pos };
		DrawCircleLinesV(pos, JUNCTION::searchDist, JUNCTION::junctionSelectionColor); 
	} else if (targetRoadPoint && !foundNewJunction) {
		auto [_, pos] = *targetRoadPoint;
		DrawCircleV(pos, JUNCTION::sizes.at(currentJunctionType), ROAD::roadSelectionColor);
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
