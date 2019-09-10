#pragma once
#include "PxPhysicsAPI.h"

#include "Structs.h"

using namespace physx;

class CollisionShape {
public:
	CollisionShape();
	void createBoxGeometry(Vector3 pSize);

	// Physics side
	PxMaterial* pMaterial = NULL;
	PxShape* pShape = NULL;

	PxGeometry* pGeometry = NULL;
};