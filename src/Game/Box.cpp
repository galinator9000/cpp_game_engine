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

void Box::createVertices() {
	// 3D Cube vertices
	Vertex _vertices[] = {
		{ -gSize.x, -gSize.y, -gSize.z },
		{ gSize.x, -gSize.y, -gSize.z },
		{ -gSize.x, gSize.y, -gSize.z },
		{ gSize.x, gSize.y, -gSize.z },
		{ -gSize.x, -gSize.y, gSize.z },
		{ gSize.x, -gSize.y, gSize.z },
		{ -gSize.x, gSize.y, gSize.z, },
		{ gSize.x, gSize.y, gSize.z }
	};
	this->vertices = _vertices;
}

void Box::createIndices() {
	// 3D Cube indices
	unsigned short _indices[] = {
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};
	this->indices = _indices;
}