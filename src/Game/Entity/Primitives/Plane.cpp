#include "Plane.h"

Plane::Plane(Vector3 material){
	this->pActor = PxCreatePlane(
		*this->ppxPhysics,
		PxPlane(0, 1, 0, 0),
		*(this->ppxPhysics->createMaterial(material.x, material.y, material.z))
	);

	// Set properties of the entity.
	this->type = ENTITY_TYPE::PLANE;
	this->isDynamic = false;
}