#pragma once
#include "PxPhysicsAPI.h"

#include "Entity/Entity.h"

using namespace physx;

class Character : public Entity{
public:
	Character(EntityProperties entityProperties) : Entity(entityProperties) {
		this->Setup();
	}

	void Setup();
	void Update();

	// Actions
	void Walk(Vector3 displacement);
	bool walkedBefore = false;

	float health = 1.0f;
	float movementSpeed = 0.6f;

	static Vector3 worldGravity;
	Timer characterTimer;
};