#include "Cube.h"

Cube::Cube(btVector3 position, btQuaternion rotation, btVector3 size, float mass) {
	this->mass = btScalar(mass);
	this->motState = new btDefaultMotionState;
	this->colShape = new btBoxShape(size);
	this->rigidBody = new btRigidBody(this->mass, this->motState, this->colShape);
	this->rigidBody->getWorldTransform().setOrigin(position);
	this->rigidBody->getWorldTransform().setRotation(rotation);
}