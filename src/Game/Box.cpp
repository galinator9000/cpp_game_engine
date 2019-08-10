#include "Box.h"

Box::Box(PxVec3 size, PxVec3 position, PxVec3 rotation, PxVec3 material) {
	this->pShape = this->ppxPhysics->createShape(
		PxBoxGeometry(size),
		*(this->ppxPhysics->createMaterial(material.x, material.y, material.z))
	);

	PxTransform transform(position);
	this->rigidDynamic = this->ppxPhysics->createRigidDynamic(transform);
	this->rigidDynamic->attachShape(*pShape);
	PxRigidBodyExt::updateMassAndInertia(*(this->rigidDynamic), 10.0f);
	this->pActor = this->rigidDynamic;

	// Graphics
	this->gSize = XMFLOAT3(size.x, size.y, size.z);
	this->gPosition = XMFLOAT3(position.x, position.y, position.z);
	this->gRotation = XMFLOAT3(rotation.x, rotation.y, rotation.z);

	this->type = ENTITY_TYPE::BOX;
}