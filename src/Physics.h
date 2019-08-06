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

private:
	float stepPerSecond;

	PxDefaultAllocator pxAllocator;
	PxDefaultErrorCallback pxErrorCallback;
	PxFoundation* pxFoundation = NULL;
	PxPhysics* pxPhysics = NULL;
	PxPvd* pxPvd = NULL;
	PxScene* pxScene = NULL;
};