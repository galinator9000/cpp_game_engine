#pragma once
#include "btBulletDynamicsCommon.h"

class BaseEntity{
public:
	btRigidBody* rigidBody;
	btScalar mass;
	btMotionState* motState;
	btCollisionShape* colShape;
};
