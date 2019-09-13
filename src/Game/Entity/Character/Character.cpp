#include "Character.h"

Vector3 Character::worldGravity;

void Character::Setup() {
	this->characterTimer.Reset();
}

// Actions
void Character::Walk(Vector3 walkDirection) {
	Vector3 displacement = walkDirection * this->movementSpeed;

	float elapsedSinceLast = 0;
	if (walkedBefore) {
		elapsedSinceLast = this->characterTimer.Peek();
	}

	PxVec3 displacementVec = PxVec3(
		displacement.x + this->worldGravity.x,
		displacement.y + this->worldGravity.y,
		displacement.z + this->worldGravity.z
	);

	PxControllerCollisionFlags pCCFlag = this->pCollisionActor->pCharacterController->move(
		displacementVec,
		0.01f,
		elapsedSinceLast,
		0,
		0
	);

	this->dataChanged = true;
	this->walkedBefore = true;
	this->characterTimer.Reset();
}