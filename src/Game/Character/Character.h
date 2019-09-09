#pragma once
#include "Structs.h"
#include "Entity/Entity.h"

class Character{
public:
	Character(Entity* entity);
	void Setup();
	void Update();

	Entity* pEntity = NULL;

	// Actions
	void Walk(Vector3 direction);

	float health = 1.0f;
	float movementSpeed = 0.6f;
};