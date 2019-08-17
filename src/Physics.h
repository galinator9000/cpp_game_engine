#pragma once
#include "Entity/BaseEntity.h"
#include "Entity/Shapes.h"

#include <Windows.h>
#include <sstream>

#include "PxPhysicsAPI.h"

using namespace physx;

class Physics{
public:
	Physics(float gravity, float stepPerSecond);
	~Physics();

	void Update();
	bool addEntity(BaseEntity* bEntity);

	// PhysX variables
public:
	PxPhysics* pxPhysics;
	PxScene* pxScene;

private:
	float stepPerSecond;

	// Default objects
	PxDefaultAllocator pxAllocator;
	PxDefaultErrorCallback pxErrorCallback;

	// PhysX main object pointers
	PxFoundation* pxFoundation;
	PxPvd* pxPvd;
};