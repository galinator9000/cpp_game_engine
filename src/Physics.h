#pragma once
#include "Entity/Entity.h"
#include "Entity/Shapes.h"

#include <Windows.h>
#include <sstream>

#include "PxPhysicsAPI.h"

using namespace physx;

class Physics{
public:
	Physics(Vector3 gravity, float stepPerSecond);
	~Physics();

	void Update();
	bool addEntity(Entity* bEntity);

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