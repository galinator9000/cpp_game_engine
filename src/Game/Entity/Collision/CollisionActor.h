#pragma once
#include "PxPhysicsAPI.h"
#include "Structs.h"

using namespace physx;

const enum COLLISION_ACTOR_TYPE {
	COLLISION_ACTOR_STATIC,
	COLLISION_ACTOR_DYNAMIC,
	COLLISION_ACTOR_KINEMATIC,
	COLLISION_ACTOR_CCT
};

class CollisionActor{
public:
	CollisionActor(COLLISION_ACTOR_TYPE actorType);

	COLLISION_ACTOR_TYPE actorType = COLLISION_ACTOR_STATIC;
	PxActor* pActor = NULL;

	// Pointer for CCT object
	PxController* pCharacterController = NULL;
};