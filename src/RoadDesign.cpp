#include <raylib.h>
#include "RoadNetwork.h"
#include "MathUtils.h"

#include <map>
#include <optional>
#include <string>

namespace ROAD {
	int intersectionRadius{ 5 };
	int maxHoveringDist{ 10 };

	Color intersectionColor{ BLUE };
	Color intersectionSelectionColor{ RAYWHITE };
	Color roadSelectionColor{ RED };

	std::map<RoadType, Color> roadColors{ {ARTERIAL, RAYWHITE},
					      {COLLECTOR, LIGHTGRAY},
					      {LOCAL, GRAY} };
}


Road::Road(int lane_count, RoadType roadType) {
	this->lanes = lane_count;
	this->classification = roadType; }


Intersection::Intersection(int ID, Vector2 pos) {
	this->pos = pos;
	this->ID = ID; };


void RoadNetwork::addIntersection(int ID, Vector2 pos) {
	Intersection newIntersection{ ID, pos }; 
	map[ID] = newIntersection; }

std::optional<int> RoadNetwork::getNewID() {
	int iteration{0};
	int maximum{ (int)1e9 };
	while (iteration < maximum) {
		int ranNum{ GetRandomValue(0, maximum) };
	
		bool isNewID{ true };
		for (auto node : map) {
			auto [ID, _] = node;
			if (ID == ranNum) isNewID = false; 
		}

		if (isNewID) return ranNum;
		iteration++;
	}

	return {};
}

std::optional<int> RoadNetwork::getNearestIntersection(Vector2 pos, int maxDist) {
	float distSquared = maxDist * maxDist;
	for (auto intersection : map) {
		Intersection* node = &intersection.second;
		float dist{ VecMath::distanceToSqr(pos, node->pos) };
		// float dist{ (pos.x - node->pos.x) * (pos.x - node->pos.x) + (pos.y - node->pos.y) * (pos.y - node->pos.x) };

		if (dist <= distSquared) { return node->ID; } }
	return {};
}


std::optional<Road::RoadPoint> RoadNetwork::getNearestRoadPoint(Vector2 pos, float maxDist) {
	float maxDistSqr{ maxDist * maxDist };

	for (auto intersection : map) {
		Intersection* start{ &intersection.second };
		for (auto edge : start->roads) {
			auto [toID, _] = edge;
			Intersection* end{ &map.at(toID) };

			Vector2 intercept{ VecMath::getIncidence(start->pos, end->pos, pos) };
			float distSqr{ VecMath::distanceToSqr(pos, intercept) };

			if (distSqr <= maxDistSqr && VecMath::isNearLine(start->pos, end->pos, pos)) {
				Road::RoadPoint point{ {start->ID, end->ID}, intercept };
				return point;
			}
		}
	}

	return {};
}

void RoadNetwork::addIntersectionToRoad(std::optional<Road::RoadPoint> roadPoint, int intersectionID) {
	if (roadPoint) {
		auto [roadIDs, roadPos] = *roadPoint;
		addIntersection(intersectionID, roadPos);
		
		auto [fromID, toID] = roadIDs;
		removeRoad(fromID, toID);

		addRoad(fromID, intersectionID, Road{});
		addRoad(intersectionID, toID, Road{});
	}
}

void RoadNetwork::removeIntersection(int ID) {
	Intersection* intersection{ &map.at(ID) };
	// for (auto road : intersection->roads) {
	// 	auto [toID, _] = road;
	// 	removeRoad(ID, toID); }

	for (auto fromNode : map) {
		auto [fromID, fromIntersection] = fromNode;

		for (auto road : fromIntersection.roads) {
			auto [toID, _] = road;
			if (toID == ID) removeRoad(fromID, toID); }
	}


	// int intersectionIndex{};
	// for (auto node : map) {
	// 	auto [intID, _] = node;
	// 	if (intID == ID) break;
	//
	// 	intersectionIndex++; }

	auto it{ map.find(ID) };
	map.erase(it);
}

void RoadNetwork::mergeIntersections(int ID1, int ID2) {
	Intersection* old1{ &map.at(ID1) };
	Intersection* old2{ &map.at(ID2) };

	std::optional<int> newID{ getNewID() };
	Intersection newIntersection{ *newID, old1->pos };

	for (auto inter : { old1, old2 }) {
		for (auto road : inter->roads) {
			auto [toID, _] = road;
			addRoad(*newID, toID, Road{});
		}
	}

	for (auto node : map) {
		auto [fromID, intersection] = node;
		for (auto road : intersection.roads) {
			auto [toID, _] = road;

			if (toID == ID1 || toID == ID2) {
				removeRoad(fromID, toID);
				addRoad(fromID, *newID, Road{});
			} 
		}
	}
}

void RoadNetwork::evalTargets(Vector2 pos) {
	intersectionTargetID = getNearestIntersection(pos, ROAD::maxHoveringDist);
	if (intersectionTargetID) {
		Intersection* node{ &map.at(*intersectionTargetID) };
		DrawCircleLines(node->pos.x, node->pos.y, ROAD::maxHoveringDist, ROAD::intersectionSelectionColor); }

	roadPoint = getNearestRoadPoint(pos, ROAD::maxHoveringDist);
	if (!intersectionTargetID && roadPoint) {
		auto [_, roadPos] = *roadPoint;
		// Vector2 roadPos = roadPoint->second;
		DrawCircle(roadPos.x, roadPos.y, ROAD::intersectionRadius, ROAD::roadSelectionColor); }
}


void RoadNetwork::addRoad(int fromID, int toID, Road road) {
	Intersection* from{ &map.at(fromID) };
	from->roads[toID] = road;
	
	// Intersection::Edge newRoad{ road, toID };
	// from -> roads.push_back(newRoad); 
}

void RoadNetwork::removeRoad(int fromID, int toID) {
	Intersection* from{ &map.at(fromID) };
	Intersection* to{ &map.at(toID) };
	
	// int roadIndex{};
	// for (auto road : from->roads) {
	// 	auto [ID, _] = road;
	// 	if (toID == ID) break;
	//
	// 	roadIndex++;
	// }

	from->roads.erase(toID);
	// from->roads.erase(from->roads.begin() + roadIndex);
}

Vector2 screenToWorldSpace(Vector2 screenPos) {
	return screenPos; 
}

Vector2 worldToScreenSpace(Vector2 worldPos) {
	return worldPos;
}


void RoadNetwork::InputHandler(int laneCount, RoadType roadType) {
	Vector2 mousePos{ screenToWorldSpace(GetMousePosition()) };
	evalTargets(mousePos);

	int LMB{ 0 };
	int RMB{ 1 };
	if (IsMouseButtonPressed(LMB)) {
		std::optional<int> newID{ getNewID() };
		if (intersectionTargetID) {
			roadStartID = *intersectionTargetID;

			Intersection* startIntersection{ &map.at(*intersectionTargetID) };
			roadStart = startIntersection->pos;
		} else if (roadPoint) {
			roadStart = roadPoint->second;
			roadStartID = *newID;

			addIntersectionToRoad(roadPoint, roadStartID);
		} else {
			roadStart = mousePos; 
			roadStartID = *newID;

			addIntersection(roadStartID, roadStart); 
		}
	} else if (IsMouseButtonDown(LMB)) {
		Color roadColor = ROAD::roadColors.at(roadType);
		if (intersectionTargetID) {
			Intersection* roadEnd{ &map.at(*intersectionTargetID) };
			DrawLineV(roadStart, roadEnd->pos, roadColor);
		} else {
			DrawLineV(roadStart, mousePos, roadColor);
			DrawCircleV(mousePos, ROAD::intersectionRadius, ROAD::intersectionColor);
		}
	} else if (IsMouseButtonReleased(LMB)) {
		Road newRoad { laneCount, roadType };
		
		if (intersectionTargetID) {
			// DrawText(std::to_string(roadStartID).c_str(), 0, 0, 100, RED);
			// DrawText(std::to_string(*intersectionTargetID).c_str(), 0, 0, 100, RED);

			addRoad(roadStartID, *intersectionTargetID, newRoad);
		} else if (roadPoint) {
			std::optional<int> newID{ getNewID() };

			addIntersectionToRoad(roadPoint, *newID);
			addRoad(roadStartID, *newID, newRoad);
		} else {
			std::optional<int> newID{ getNewID() };
			addIntersection(*newID, mousePos);
			addRoad(roadStartID, *newID, newRoad); 
		}
		roadStartID = -1;
	} else if (IsKeyPressed(KEY_D) && intersectionTargetID) {
		removeIntersection(*intersectionTargetID);
	}


	if (IsMouseButtonPressed(RMB) && intersectionTargetID) {
		currentTarget = &map.at(*intersectionTargetID);
	} else if (IsMouseButtonReleased(RMB)) {
		// if (intersectionTargetID) {
		// 	mergeIntersections(*intersectionTargetID, currentTarget->ID);
		// }

		currentTarget = nullptr;
	}

	if (currentTarget) {
		(*currentTarget).pos.x = GetMouseX();
		(*currentTarget).pos.y = GetMouseY();
	}
}

void RoadNetwork::draw(Vector2 screenOrigin) {
	int nodeCounter{};
	for (auto node : map) {
		auto [ID, intersection] = node; 

		Vector2 intersectionPos{ VecMath::add(screenOrigin, worldToScreenSpace(intersection.pos)) };
		DrawCircleV(intersectionPos, ROAD::intersectionRadius, ROAD::intersectionColor);
	
		for (auto road : intersection.roads) {
			auto [toID, roadData] = road;
			Intersection* to{ &map.at(toID) };

			Color roadColor = ROAD::roadColors.at(roadData.classification);
			Vector2 toPos{ VecMath::add(screenOrigin, worldToScreenSpace(to->pos)) };

			DrawLineV(intersectionPos, toPos, roadColor);
			// DrawText(std::to_string(roadData.classification).c_str(), 20, 20*(nodeCounter+1), 20, RED);
		}

		// DrawText(std::to_string(intersection.ID).c_str(), 20, 20*(nodeCounter+1), 20, RED);

		nodeCounter++;
	} }
