#include "Box.h"

Box::Box(Vector3 size, Vector3 position, Vector4 rotationQ, Color color, Vector3 material, Mesh* mesh) {
	// Check initial quaternion vector.
	if (rotationQ == Vector4(0, 0, 0, 0)) {
		rotationQ.x = 1.0f;
	}

	this->gSize = dx::XMFLOAT3(size.x, size.y, size.z);
	this->gPosition = dx::XMFLOAT3(position.x, position.y, position.z);
	this->gRotationQ = dx::XMFLOAT4(rotationQ.x, rotationQ.y, rotationQ.z, rotationQ.w);

	// Graphics
	this->entityMaterial.color = color;
	this->entityMaterial.specularPower = 3.0f;
	this->entityMaterial.specularIntensity = 0.5f;

	// Attach mesh if it is given to constructor.
	if (mesh != NULL) {
		this->attachMesh(mesh);
	}

	this->gSize = XMFLOAT3(size.x, size.y, size.z);
	this->gPosition = XMFLOAT3(position.x, position.y, position.z);
	this->gRotationQ = XMFLOAT4(rotationQ.x, rotationQ.y, rotationQ.z, rotationQ.w);

	// Set properties of the entity.
	this->isDynamic = true;

	this->updateConstantBuffer();
}

void Box::Update(){
	// Skip static and sleeping dynamic entities.
	if(!this->isDynamic){
		this->shouldUpdateGPUData = false;
		return;
	}
	// Dynamic and sleeping check.
	if(this->isDynamic && this->rigidDynamic->isSleeping()){
		this->shouldUpdateGPUData = false;
		return;
	}

	// Integrate entities' physics position and rotation with graphics side.
	PxTransform tm = this->rigidDynamic->getGlobalPose();
	this->gPosition.x = tm.p.x;
	this->gPosition.y = tm.p.y;
	this->gPosition.z = tm.p.z;

	// Get quaternion vector as radians.
	PxVec3 axisVector;
	tm.q.toRadiansAndUnitAxis(this->gRotationQ.w, axisVector);

	this->gRotationQ.x = axisVector.x;
	this->gRotationQ.y = axisVector.y;
	this->gRotationQ.z = axisVector.z;

	this->updateConstantBuffer();
}