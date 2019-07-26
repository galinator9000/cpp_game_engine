#pragma once
#include "btBulletDynamicsCommon.h"

class Physics{
public:
	Physics(float Gravity, float stepPerSecond);
	void Update();

private:
	btDiscreteDynamicsWorld* World;
	float stepPerSecond;
};