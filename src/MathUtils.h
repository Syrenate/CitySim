#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <raylib.h>

namespace VecMath {
	float distanceToSqr(Vector2 v1, Vector2 v2);
	Vector2 getIncidence(Vector2 from, Vector2 to, Vector2 pos);
	float dotProduct(Vector2 v1, Vector2 v2);
	bool isNearLine(Vector2 from, Vector2 to, Vector2 pos);

	Vector2 add(Vector2 v1, Vector2 v2);
	Vector2 sub(Vector2 v1, Vector2 v2);
}

#endif
