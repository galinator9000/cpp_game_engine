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
	this->gColor = XMFLOAT3(color.r, color.g, color.b);
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

	// Update PS constant buffer.
	this->gEntityPSConstantBuffer.color = this->gColor;
	this->gEntityPSConstantBuffer.useTexture = this->useTexture;

	// Graphics object will check this if buffer should be updated or not.
	this->shouldUpdateGPUData = true;
}