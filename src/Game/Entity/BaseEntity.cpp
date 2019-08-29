#include "BaseEntity.h"
#include "Shapes.h"

PxPhysics* BaseEntity::ppxPhysics;

BaseEntity::~BaseEntity() {
	delete[] this->gVertices;
	delete[] this->gIndices;
}

void BaseEntity::attachTextureAndSampler(Texture* texture, TextureSampler* textureSampler){
	this->texture = texture;
	this->textureSampler = textureSampler;
	this->useTexture = true;

	this->dataChanged = true;
}

void BaseEntity::detachTextureAndSampler() {
	this->texture = NULL;
	this->textureSampler = NULL;
	this->useTexture = false;

	this->dataChanged = true;
}

void BaseEntity::setColor(Color color) {
	this->entityMaterial.color = color;
	this->dataChanged = true;
}

void BaseEntity::Reset() {
	this->dataChanged = false;
	this->shouldUpdateGPUData = false;
}

void BaseEntity::updateConstantBuffer(){
	// Update VS constant buffer.
	dx::XMStoreFloat4x4(
		&(this->gEntityVSConstantBuffer.worldMatrix),
		dx::XMMatrixTranspose(
			dx::XMMatrixScaling(this->gSize.x, this->gSize.y, this->gSize.z) *
			dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&this->gRotationQ)) *
			dx::XMMatrixTranslation(this->gPosition.x, this->gPosition.y, this->gPosition.z)
		)
	);

	//// Update PS constant buffer.
	// Material information.
	this->gEntityPSConstantBuffer.color = XMFLOAT3(
		this->entityMaterial.color.r,
		this->entityMaterial.color.g,
		this->entityMaterial.color.b
	);
	this->gEntityPSConstantBuffer.useTexture = this->useTexture;

	this->gEntityPSConstantBuffer.specularHighlight.x = this->entityMaterial.specularPower;
	this->gEntityPSConstantBuffer.specularHighlight.y = this->entityMaterial.specularIntensity;

	// Graphics object will check this if buffer should be updated or not.
	this->shouldUpdateGPUData = true;
}

void BaseEntity::Translate(Vector3 translationVector){
	this->gPosition.x += translationVector.x;
	this->gPosition.y += translationVector.y;
	this->gPosition.z += translationVector.z;
	this->dataChanged = true;
}

void BaseEntity::Rotate(Vector3 rotationVector){
	this->dataChanged = true;
}

void BaseEntity::Scale(Vector3 scalingVector){
	this->gSize.x += scalingVector.x;
	this->gSize.y += scalingVector.y;
	this->gSize.z += scalingVector.z;
	this->dataChanged = true;
}