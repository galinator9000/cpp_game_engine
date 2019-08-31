#include "Box.h"

Box::Box(Vector3 size, Vector3 position, Vector4 rotationQ, Color color, Vector3 material) {
	this->pShape = this->ppxPhysics->createShape(
		PxBoxGeometry(PxVec3(size.x, size.y, size.z)),
		*(this->ppxPhysics->createMaterial(material.x, material.y, material.z))
	);

	PxTransform transform(PxVec3(position.x, position.y, position.z));
	this->rigidDynamic = this->ppxPhysics->createRigidDynamic(transform);
	this->rigidDynamic->attachShape(*pShape);
	PxRigidBodyExt::updateMassAndInertia(*(this->rigidDynamic), 10.0f);
	this->pActor = this->rigidDynamic;

	// Graphics
	this->entityMaterial.color = color;
	this->entityMaterial.specularPower = 3.0f;
	this->entityMaterial.specularIntensity = 0.5f;

	this->gSize = XMFLOAT3(size.x, size.y, size.z);
	this->gPosition = XMFLOAT3(position.x, position.y, position.z);
	this->gRotationQ = XMFLOAT4(rotationQ.x, rotationQ.y, rotationQ.z, rotationQ.w);

	// Set properties of the entity.
	this->isDynamic = true;

	this->updateConstantBuffer();
}

void Box::Update(){
	// Skip static and sleeping dynamic entities.
	if(!this->dataChanged){
		// Static check.
		if(!this->isDynamic){
			this->shouldUpdateGPUData = false;
			return;
		}
		// Dynamic and sleeping check.
		if(this->isDynamic && this->rigidDynamic->isSleeping()){
			this->shouldUpdateGPUData = false;
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