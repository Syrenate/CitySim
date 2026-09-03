#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <raylib.h>

namespace VecMath {
	float distanceToSqr(Vector2 v1, Vector2 v2);
	Vector2 getIncidence(Vector2 from, Vector2 to, Vector2 pos);
	float dotProduct(Vector2 v1, Vector2 v2);
	bool isNearLine(Vector2 from, Vector2 to, Vector2 pos);
	float distance(Vector2 v1, Vector2 v2);

	Vector2 add(Vector2 v1, Vector2 v2);
	Vector2 sub(Vector2 v1, Vector2 v2);

	float length(Vector2 v);
	Vector2 normalize(Vector2 v);
	Vector2 scale(Vector2 v, float scaleFactor);
	Vector2 resize(Vector2 v, float length);
	Vector2 rotate(Vector2 v, float angle);
}

#endif
