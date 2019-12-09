#pragma once
#include "PxPhysicsAPI.h"

#include "Entity/Entity.h"

using namespace physx;

class Character : public Entity{
public:
	Character(
		Vector3 size = Vector3(1, 1, 1), Vector3 position = Vector3(0, 0, 0), Vector4 rotationQ = Vector4(1, 0, 0, 0),
		Material* material = NULL, CollisionMaterial* collisionMaterial = NULL,
		Mesh* pMesh = NULL,
		CollisionShape* pCollisionShape = NULL,
		CollisionActor* pCollisionActor = NULL,

		float movementSpeed = 0.1f,
		Vector3 facingDirection = { 0, 0, 1 }) : Entity(size, position, rotationQ, material, collisionMaterial, pMesh, pCollisionShape, pCollisionActor
	) {
		this->facingDirection = facingDirection;
		this->movementSpeed = movementSpeed;

		this->Setup();
	}

	void Setup();

	// General values.
	float health = 1.0f;
	Vector3 facingDirection;

	// Actions
	void Walk(Vector3 displacement);

	// Physics
	PxControllerFilters* cctControllerFilters;
	static Vector3 worldGravity;

private:
	float movementSpeed = 60.0f;
	bool walkedBefore = false;
	Timer characterTimer;
};