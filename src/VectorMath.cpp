#include <raylib.h>
#include <cmath>
#include "MathUtils.h"

float VecMath::distanceToSqr(Vector2 v1, Vector2 v2) {
	return (v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y);
}

Vector2 VecMath::getIncidence(Vector2 from, Vector2 to, Vector2 pos) {
	float gradient{ (to.y - from.y) / (to.x - from.x) };
	float intercept1{ from.y - gradient * from.x };
	float intercept2{ pos.y + pos.x / gradient };

	float x0{ (intercept2 - intercept1) / (gradient + 1 / gradient) };
	float y0{ intercept1 + gradient * x0 };

	return Vector2{ x0, y0 };
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
