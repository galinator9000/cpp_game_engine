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
	this->Update();
}

Box::~Box(){}

void Box::Update(){
	// Integrate entities' physics position and rotation with graphics side.
	PxTransform tm = this->rigidDynamic->getGlobalPose();
	this->gPosition.x = tm.p.x;
	this->gPosition.y = tm.p.y;
	this->gPosition.z = tm.p.z;

	float angleX = 4.0f;
	PxVec3 axisX(1.0f, 0.0f, 0.0f);
	tm.q.toRadiansAndUnitAxis(angleX, axisX);

	float angleY = 3.0f;
	PxVec3 axisY(0.0f, 1.0f, 0.0f);
	tm.q.toRadiansAndUnitAxis(angleY, axisY);

	float angleZ = 2.0f;
	PxVec3 axisZ(0.0f, 0.0f, 1.0f);
	tm.q.toRadiansAndUnitAxis(angleZ, axisZ);

	this->gRotation.x = tm.q.x;
	this->gRotation.y = tm.q.y;
	this->gRotation.z = tm.q.z;

	this->updateConstantBuffer();
}

void Box::updateConstantBuffer() {
	// Update constant buffer.
	dx::XMStoreFloat4x4(
		&(this->gConstBuffer.transform),
		dx::XMMatrixTranspose(
			dx::XMMatrixRotationZ(this->gRotation.z) *
			dx::XMMatrixRotationY(this->gRotation.y) *
			dx::XMMatrixRotationX(this->gRotation.x) *
			dx::XMMatrixTranslation(this->gPosition.x, this->gPosition.y, this->gPosition.z) *
			dx::XMMatrixPerspectiveLH(1.0f, 1.0f, 0.5f, 10.0f)
		)
	);
}

void Box::gCreateVerticesAndIndices() {
	// 3D Cube vertices
	Vertex _vertices[] = {
		{{ -gSize.x, -gSize.y, -gSize.z }},
		{{ gSize.x, -gSize.y, -gSize.z }},
		{{ -gSize.x, gSize.y, -gSize.z }},
		{{ gSize.x, gSize.y, -gSize.z }},
		{{ -gSize.x, -gSize.y, gSize.z }},
		{{ gSize.x, -gSize.y, gSize.z }},
		{{ -gSize.x, gSize.y, gSize.z, }},
		{{ gSize.x, gSize.y, gSize.z }}
	};
	this->gVertexCount = (UINT) std::size(_vertices);

	Vertex* vertices = new Vertex[this->gVertexCount];
	std::copy(_vertices, _vertices + this->gVertexCount, vertices);
	this->gVertices = vertices;
	
	// 3D Cube indices
	unsigned short _indices[] = {
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};
	this->gIndexCount = (UINT) std::size(_indices);

	unsigned short* indices = new unsigned short[this->gIndexCount];
	std::copy(_indices, _indices + this->gIndexCount, indices);
	this->gIndices = indices;
}