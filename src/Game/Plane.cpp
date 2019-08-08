#include "Plane.h"

Plane::Plane(PxVec3 material){
	this->actor = PxCreatePlane(
		*this->ppxPhysics,
		PxPlane(0, 1, 0, 0),
		*(this->ppxPhysics->createMaterial(material.x, material.y, material.z))
	);

	this->entityId = BaseEntity::lastEntityId;
	BaseEntity::lastEntityId += 1;
}