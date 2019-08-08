#pragma once
#include "BaseEntity.h"

#include <Windows.h>
#include <sstream>

#include "PxPhysicsAPI.h"
using namespace physx;

class Physics{
public:
	Physics(float Gravity, float stepPerSecond);
	~Physics();

	void Update();
	void AddEntity(BaseEntity& pBaseEntity);

	// PhysX variables
public:
	PxPhysics* pxPhysics;
private:
	float stepPerSecond;

	// Default objects
	PxDefaultAllocator pxAllocator;
	PxDefaultErrorCallback pxErrorCallback;

	// PhysX object pointers
	PxFoundation* pxFoundation;
	PxPvd* pxPvd;
	PxScene* pxScene;
};