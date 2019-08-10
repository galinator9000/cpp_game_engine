#pragma once
#include <DirectXMath.h>
#include "PxPhysicsAPI.h"

using namespace physx;
namespace dx = DirectX;

const enum ENTITY_TYPE{
	BOX,
	PLANE
};

class BaseEntity{
public:
	// Derived classes will set this value.
	unsigned int id;
	unsigned int type;

	// Physics
	static PxPhysics* ppxPhysics;
	PxShape* pShape = NULL;
	PxActor* pActor = NULL;

	// Graphics
	dx::XMFLOAT3 gSize;
	dx::XMFLOAT3 gPosition;
	dx::XMFLOAT3 gRotation;
};
