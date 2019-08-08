#pragma once
#include "BaseEntity.h"

class Box : public BaseEntity {
public:
	Box(PxVec3 scale, PxVec3 position, PxVec3 material);
	PxRigidDynamic* rigidDynamic;
};