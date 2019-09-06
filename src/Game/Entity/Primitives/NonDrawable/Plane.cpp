#include "Plane.h"

Plane::Plane(Vector3 normals, Vector3 material){
	this->pActor = PxCreatePlane(
		*this->ppxPhysics,
		PxPlane(normals.x, normals.y, normals.z, 0),
		*(this->ppxPhysics->createMaterial(material.x, material.y, material.z))
	);

	// Set properties of the entity.
	this->isDynamic = false;
}