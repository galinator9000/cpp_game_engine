#pragma once
#include "Entity/Entity.h"

class Character : public Entity{
public:
	Character(EntityProperties entityProperties) : Entity(entityProperties) {
		this->Setup();
	}

	void Setup();
	void Update();

	// Actions
	void Walk(Vector3 direction);

	float health = 1.0f;
	float movementSpeed = 0.6f;
};