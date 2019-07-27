#pragma once
#include "btBulletDynamicsCommon.h"
#include "BaseEntity.h"
#include <Windows.h>
#include <sstream>

class Physics{
public:
	Physics(float Gravity, float stepPerSecond);
	void Update();
	void AddEntity(BaseEntity& pBaseEntity);

private:
	btDiscreteDynamicsWorld* phyWorld;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	float stepPerSecond;
};