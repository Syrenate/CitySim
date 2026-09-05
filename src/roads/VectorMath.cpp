#include <raylib.h>
#include <cmath>
#include <optional>
#include "MathUtils.h"

float VecMath::distanceToSqr(Vector2 v1, Vector2 v2) {
	return (v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y);
}

float VecMath::getGradient(Vector2 from, Vector2 to) {
	return (to.y - from.y) / (to.x - from.x);
}

Vector2 VecMath::getIncidence(Vector2 from, Vector2 to, Vector2 pos) {
	float gradient{ VecMath::getGradient(from, to) };
	float intercept1{ from.y - gradient * from.x };
	float intercept2{ pos.y + pos.x / gradient };

	float x0{ (intercept2 - intercept1) / (gradient + 1 / gradient) };
	float y0{ intercept1 + gradient * x0 };

	return Vector2{ x0, y0 };
}

std::optional<Vector2> VecMath::getIntersection(Vector2 from1, Vector2 to1, Vector2 from2, Vector2 to2) {
	if (from1.x == to1.x) {
		if (from2.x == to2.x) return {};

		float grad2{ VecMath::getGradient(from2, to2) };
		float intercept2{ to2.y - grad2 * to2.x };

		return Vector2{ from1.x, grad2 * from1.x + intercept2 };
	} else if (from2.x == to2.x) {
		if (from1.x == to1.x) return {};

		float grad1{ VecMath::getGradient(from1, to1) };
		float intercept1{ to1.y - grad1 * to1.x };

		return Vector2{ from2.x, grad1 * from2.x + intercept1 };	
	}

	float grad1{ VecMath::getGradient(from1, to1) };
	float grad2{ VecMath::getGradient(from2, to2) };

	if (grad1 == grad2) return {};


	float intercept1{ to1.y - grad1 * to1.x };
	float intercept2{ to2.y - grad2 * to2.x };

	float x0{ (intercept2 - intercept1) / (grad1 - grad2) };
	float y0{ grad1 * x0 + intercept1 };
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

bool VecMath::isOnLine(Vector2 v1, Vector2 v2, Vector2 pos) {
	float grad{ getGradient(v1, v2) };
	float intercept{ v1.y - grad * v1.x };

	return (isInBounds(v1,v2,pos) && pos.y == grad * pos.x + intercept);
}

bool VecMath::isInBounds(Vector2 v1, Vector2 v2, Vector2 pos) {
	Vector2 xBounds{ std::min(v1.x, v2.x), std::max(v1.x, v2.x) };
	Vector2 yBounds{ std::min(v1.y, v2.y), std::max(v1.y, v2.y) };

	return (pos.x >= xBounds.x) && (pos.x <= xBounds.y) &&
	       (pos.y >= yBounds.x) && (pos.y <= yBounds.y);
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
