#pragma once
#include "PxPhysicsAPI.h"
using namespace physx;

class BaseEntity{
public:
	// Index to keep track of entity id's.
	static unsigned int lastEntityId;

	// Derived classes will set this value.
	unsigned int entityId;

	// PhysX
	static PxPhysics* ppxPhysics;
	PxShape* shape = NULL;
	PxActor* actor = NULL;
};
