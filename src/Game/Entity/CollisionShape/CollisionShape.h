#pragma once
#include "PxPhysicsAPI.h"

using namespace physx;

class CollisionShape {
public:
	CollisionShape();
	CollisionShape(PxShape* pShape);

	PxShape* pShape;
};