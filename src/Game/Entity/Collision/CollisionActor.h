#pragma once
#include "PxPhysicsAPI.h"
#include "Structs.h"

using namespace physx;

class CollisionActor{
public:
	CollisionActor(COLLISION_ACTOR_TYPE actorType);

	COLLISION_ACTOR_TYPE actorType = COLLISION_ACTOR_STATIC;
	PxActor* pActor = NULL;

	void setPosition(Vector3 position);
	void setRotation(Vector4 rotationQ);
	void setMass(float mass);
	void setVelocity(Vector3 velocityVec);

	// Ragdoll
	CollisionActor* parentActor = NULL;
	PxTransform initialTransform;
	PxTransform jointPointParent;
	PxTransform jointPointSelf;
	float boneLength = 0.0f;

	// Pointer for CCT object
	PxController* pCharacterController = NULL;
};