#pragma once
#include "PxPhysicsAPI.h"

using namespace physx;

class CollisionActor{
public:
	CollisionActor(bool isDynamic);

	bool isDynamic = false;
	PxActor* pActor = NULL;
};