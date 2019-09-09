#include "Character.h"

Character::Character(Entity* entity) {
	this->pEntity = entity;

	this->Setup();
}

void Character::Setup() {

}

void Character::Update() {

}

// Actions
void Character::Walk(Vector3 direction) {
	this->pEntity->Translate(direction * this->movementSpeed);
}