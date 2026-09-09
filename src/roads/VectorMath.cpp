#include <raylib.h>
#include <cmath>
#include <optional>
#include <string>
#include <iostream>
#include "MathUtils.h"

bool VecMath::equal(Vector2 v1, Vector2 v2) {
	return (v1.x == v2.x && v1.y == v2.y); }

float VecMath::distanceToSqr(Vector2 v1, Vector2 v2) {
	return (v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y);
}

float VecMath::getGradient(Vector2 from, Vector2 to) {
	return (to.y - from.y) / (to.x - from.x);
}

Vector2 VecMath::getIncidence(Vector2 from, Vector2 to, Vector2 pos) {
	if (from.x == to.x) return Vector2{ from.x, pos.y };
	if (from.y == to.y) return Vector2{ pos.x, from.y };

	float gradient{ VecMath::getGradient(from, to) };
	float intercept1{ from.y - gradient * from.x };
	float intercept2{ pos.y + pos.x / gradient };

	float x0{ (intercept2 - intercept1) / (gradient + 1 / gradient) };
	float y0{ intercept1 + gradient * x0 };

	return Vector2{ x0, y0 };
}

std::optional<Vector2> VecMath::getIntersection(Vector2 from1, Vector2 to1, Vector2 from2, Vector2 to2) {
	Vector2 collision{};
	CheckCollisionLines(from1, to1, from2, to2, &collision);
	return collision;
}

float VecMath::dotProduct(Vector2 v1, Vector2 v2) {
	return (v1.x * v2.x) + (v1.y * v2.y);
}

bool VecMath::isNearLine(Vector2 from, Vector2 to, Vector2 pos) {
	Vector2 roadVector{ to.x - from.x, to.y - from.y };
	Vector2 startToPos{ pos.x - from.x, pos.y - from.y };
	Vector2 endToPos{ pos.x - to.x, pos.y - to.y };

	Vector2 invRoadVector{ -roadVector.x, -roadVector.y };
	return (VecMath::dotProduct(roadVector, startToPos) > 0 && VecMath::dotProduct(invRoadVector, endToPos) > 0);
}

bool VecMath::isOnLine(Vector2 v1, Vector2 v2, Vector2 pos) {
	float grad{ getGradient(v1, v2) };
	float intercept{ v1.y - grad * v1.x };

	// return (isInBounds(v1,v2,pos) && CheckCollisionCircleLine(pos, 1, v1, v2));
	return (isInBounds(v1,v2,pos) && pos.y == grad * pos.x + intercept);
}

bool VecMath::isInBounds(Vector2 v1, Vector2 v2, Vector2 pos) {
	Vector2 xBounds{ std::min(v1.x, v2.x), std::max(v1.x, v2.x) };
	Vector2 yBounds{ std::min(v1.y, v2.y), std::max(v1.y, v2.y) };

	float buffer{ 0.5 };
	return (pos.x >= xBounds.x - buffer) && (pos.x <= xBounds.y + buffer) &&
	       (pos.y >= yBounds.x - buffer) && (pos.y <= yBounds.y + buffer);
}

float VecMath::distance(Vector2 v1, Vector2 v2) {
	return VecMath::length(VecMath::sub(v1, v2)); }

Vector2 VecMath::add(Vector2 v1, Vector2 v2) {
	return Vector2{ v1.x + v2.x, v1.y + v2.y };
}

Vector2 VecMath::sub(Vector2 v1, Vector2 v2) {
	return Vector2{ v1.x - v2.x, v1.y - v2.y };
}

float VecMath::length(Vector2 v) {
	return std::sqrt(v.x*v.x + v.y*v.y);
}

Vector2 VecMath::normalize(Vector2 v) {
	float length{ VecMath::length(v) };
	return Vector2{ v.x / length, v.y / length };
}

Vector2 VecMath::scale(Vector2 v, float scaleFactor) {
	return Vector2{ v.x * scaleFactor, v.y * scaleFactor };
}

Vector2 VecMath::resize(Vector2 v, float length) {
	return VecMath::scale(VecMath::normalize(v), length);
}

Vector2 VecMath::rotate(Vector2 v, float angle) {
	return Vector2{ float(std::cos(angle)*v.x + sin(angle)*v.y), float(-sin(angle)*v.x + cos(angle)*v.y) };
}

Vector2 VecMath::snapPosition(Vector2 start, Vector2 end, int divisions) {
	Vector2 vec{ VecMath::sub(end, start) };
	double currentAngle{ atan(vec.y / vec.x) };

	double correctAngle{ (vec.x == 0) ? (vec.y >= 0 ? PI/2 : -PI/2) :
			     (vec.x < 0 && vec.y >= 0) ? -PI + currentAngle : 
			     (vec.x < 0 && vec.y < 0) ? PI + currentAngle : currentAngle };
	double roundedAngle{ 2*PI * round(divisions * correctAngle / (2*PI)) / divisions };

	float length{ VecMath::length(vec) };
	return Vector2{ float(round(start.x + length * cos(roundedAngle))), 
			float(round(start.y + length * sin(roundedAngle))) };
}
