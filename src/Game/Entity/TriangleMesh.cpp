#include "TriangleMesh.h"

// Utils
//#include "fbxsdk.h"

TriangleMesh::TriangleMesh(Vector3 size, Vector3 position, Vector3 rotation, Vector3 material){
	PxQuat rotationQuaternion;

	// Graphics
	this->gSize = XMFLOAT3(size.x, size.y, size.z);
	this->gPosition = XMFLOAT3(position.x, position.y, position.z);
	this->gRotationQ = XMFLOAT4(rotationQuaternion.x, rotationQuaternion.y, rotationQuaternion.z, rotationQuaternion.w);

	// Set properties of the entity.
	this->type = ENTITY_TYPE::TRIANGLE_MESH;
	this->isDynamic = false;

	this->Update(true);
}

TriangleMesh::~TriangleMesh() {}

void TriangleMesh::Update(bool initial) {
	// Skip static and sleeping dynamic entities.
	this->shouldUpdateData = true;

	if (!initial) {
		// Static check.
		if (!this->isDynamic) {
			this->shouldUpdateData = false;
			return;
		}
	}

	this->updateConstantBuffer();
}

void TriangleMesh::updateConstantBuffer() {
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

bool TriangleMesh::LoadVerticesAndIndicesFBX(const char* fileName){
	return false;
}