#include "Box.h"

Box::Box(PxVec3 scale, PxVec3 position, PxVec3 material){
	this->shape = this->ppxPhysics->createShape(
		PxBoxGeometry(scale),
		*(this->ppxPhysics->createMaterial(material.x, material.y, material.z))
	);

	PxTransform transform(position);
	this->rigidDynamic = this->ppxPhysics->createRigidDynamic(transform);
	this->rigidDynamic->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*(this->rigidDynamic), 10.0f);

	this->actor = this->rigidDynamic;
	this->entityId = BaseEntity::lastEntityId;
	BaseEntity::lastEntityId += 1;
}