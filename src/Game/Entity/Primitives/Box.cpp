#include "Box.h"

Box::Box(Vector3 size, Vector3 position, Vector3 rotation, Vector3 material) {
	this->pShape = this->ppxPhysics->createShape(
		PxBoxGeometry(PxVec3(size.x, size.y, size.z)),
		*(this->ppxPhysics->createMaterial(material.x, material.y, material.z))
	);

	PxQuat rotationQuaternion;
	PxTransform transform(PxVec3(position.x, position.y, position.z));
	this->rigidDynamic = this->ppxPhysics->createRigidDynamic(transform);
	this->rigidDynamic->attachShape(*pShape);
	PxRigidBodyExt::updateMassAndInertia(*(this->rigidDynamic), 10.0f);
	this->pActor = this->rigidDynamic;

	// Graphics
	this->gSize = XMFLOAT3(size.x, size.y, size.z);
	this->gPosition = XMFLOAT3(position.x, position.y, position.z);
	//this->gRotationQ = XMFLOAT4(rotationQuaternion.x, rotationQuaternion.y, rotationQuaternion.z, rotationQuaternion.w);
	this->gRotationQ = XMFLOAT4(0, 0, 0, 0);

	// Set properties of the entity.
	this->type = ENTITY_TYPE::BOX;
	this->isDynamic = true;

	this->Update(true);
}

Box::~Box(){}

void Box::Update(bool initial){
	// Skip static and sleeping dynamic entities.
	this->shouldUpdateData = true;

	if(!initial){
		// Static check.
		if(!this->isDynamic){
			this->shouldUpdateData = false;
			return;
		}
		// Dynamic and sleeping check.
		if(this->isDynamic && this->rigidDynamic->isSleeping()){
			this->shouldUpdateData = false;
			return;
		}
	}

	// Integrate entities' physics position and rotation with graphics side.
	PxTransform tm = this->rigidDynamic->getGlobalPose();
	this->gPosition.x = tm.p.x;
	this->gPosition.y = tm.p.y;
	this->gPosition.z = tm.p.z;
	this->gRotationQ.x = tm.q.x;
	this->gRotationQ.y = tm.q.y;
	this->gRotationQ.z = tm.q.z;
	this->gRotationQ.w = tm.q.w;	

	this->updateConstantBuffer();
}

void Box::updateConstantBuffer() {
	// Update constant buffer that held on class.
	dx::XMStoreFloat4x4(
		&(this->gConstBuffer.worldMatrix),
		dx::XMMatrixTranspose(
			dx::XMMatrixScaling(this->gSize.x, this->gSize.y, this->gSize.z) *
			dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&this->gRotationQ)) *
			dx::XMMatrixTranslation(this->gPosition.x, this->gPosition.y, this->gPosition.z)
		)
	);
}

void Box::gCreateVerticesAndIndices() {
	// 3D Cube vertices
	Vertex _vertices[] = {
		{{ -1.0f, -1.0f, -1.0f }},
		{{ 1.0f, -1.0f, -1.0f }},
		{{ -1.0f, 1.0f, -1.0f }},
		{{ 1.0f, 1.0f, -1.0f }},
		{{ -1.0f, -1.0f, 1.0f }},
		{{ 1.0f, -1.0f, 1.0f }},
		{{ -1.0f, 1.0f, 1.0f, }},
		{{ 1.0f, 1.0f, 1.0f }}
	};
	this->gVertexCount = (UINT) std::size(_vertices);

	Vertex* vertices = new Vertex[this->gVertexCount];
	std::copy(_vertices, _vertices + this->gVertexCount, vertices);
	this->gVertices = vertices;
	
	// 3D Cube indices
	unsigned int _indices[] = {
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};
	this->gIndexCount = (UINT) std::size(_indices);

	unsigned int* indices = new unsigned int[this->gIndexCount];
	std::copy(_indices, _indices + this->gIndexCount, indices);
	this->gIndices = indices;
}