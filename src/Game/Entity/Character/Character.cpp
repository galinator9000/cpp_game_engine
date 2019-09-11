#include "Character.h"

void Character::Setup() {

}

void Character::Update() {

}

// Actions
void Character::Walk(Vector3 direction) {
	this->Translate(direction * this->movementSpeed);
}