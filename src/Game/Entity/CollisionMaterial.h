#pragma once
#include "Structs.h"

class CollisionMaterial {
public:
	CollisionMaterial(
		float staticFriction = 1.0f,
		float dynamicFriction = 1.0f,
		float restitution = 0.0f,
		float density = 10.0f
	) {
		this->staticFriction = staticFriction;
		this->dynamicFriction = dynamicFriction;
		this->restitution = restitution;
		this->density = density;
	}

	float staticFriction = 1.0f;
	float dynamicFriction = 1.0f;
	float restitution = 0.0f;
	float density = 10.0f;
};