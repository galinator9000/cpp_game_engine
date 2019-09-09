#pragma once
#include "PxPhysicsAPI.h"

#include "Structs.h"

using namespace physx;

class CollisionShape {
public:
	CollisionShape(Vector3 pSize);
	void createBoxGeometry();

	Vector3 pSize;

	// Physics side
	PxMaterial* pMaterial;
	PxGeometry* pGeometry;
	PxShape* pShape;
};