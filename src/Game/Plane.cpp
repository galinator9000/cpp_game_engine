#include "Plane.h"

Plane::Plane(PxVec3 material){
	this->pActor = PxCreatePlane(
		*this->ppxPhysics,
		PxPlane(0, 1, 0, 0),
		*(this->ppxPhysics->createMaterial(material.x, material.y, material.z))
	);

	this->type = ENTITY_TYPE::PLANE;
}