#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <raylib.h>
#include <optional>

namespace VecMath {
	Vector2 add(Vector2 v1, Vector2 v2);
	Vector2 sub(Vector2 v1, Vector2 v2);

	float length(Vector2 v);
	Vector2 normalize(Vector2 v);
	Vector2 scale(Vector2 v, float scaleFactor);
	Vector2 resize(Vector2 v, float length);
	Vector2 rotate(Vector2 v, float angle);

	float dotProduct(Vector2 v1, Vector2 v2);
	float getGradient(Vector2 from, Vector2 to);
	std::optional<Vector2> getIntersection(Vector2 from1, Vector2 to1, Vector2 from2, Vector2 to2);
	Vector2 getIncidence(Vector2 from, Vector2 to, Vector2 pos);

	bool isOnLine(Vector2 v1, Vector2 v2, Vector2 pos);
	bool isInBounds(Vector2 v1, Vector2 v2, Vector2 pos);
	bool isNearLine(Vector2 from, Vector2 to, Vector2 pos);

	float distance(Vector2 v1, Vector2 v2);
	float distanceToSqr(Vector2 v1, Vector2 v2);


}

#endif
