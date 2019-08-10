#pragma once
#include "BaseEntity.h"

using namespace DirectX;

class Box : public BaseEntity {
public:
	Box(PxVec3 size, PxVec3 position, PxVec3 rotation, PxVec3 material);

	// Physics
	PxRigidDynamic* rigidDynamic;

	// Graphics
};