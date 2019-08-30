#include "Entity.h"
#include "Shapes.h"

PxPhysics* Entity::ppxPhysics;

// Default constructor.
Entity::Entity(){}

Entity::Entity(Vector3 size, Vector3 position, Vector3 rotation, Color color, Vector3 material) {
	PxQuat rotationQuaternion;

	// Graphics
	this->entityMaterial.color = color;
	this->entityMaterial.specularPower = 15.0f;
	this->entityMaterial.specularIntensity = 3.0f;

	this->gSize = dx::XMFLOAT3(size.x, size.y, size.z);
	this->gPosition = dx::XMFLOAT3(position.x, position.y, position.z);
	//this->gRotationQ = dx::XMFLOAT4(rotationQuaternion.x, rotationQuaternion.y, rotationQuaternion.z, rotationQuaternion.w);
	this->gRotationQ = dx::XMFLOAT4(0, 0, 0, 0);

	// Set properties of the entity.
	this->isDynamic = false;

	this->updateConstantBuffer();
}

void Entity::Update() {
	// Skip static and sleeping dynamic entities.
	if (!this->dataChanged) {
		// Static check.
		if (!this->isDynamic) {
			this->shouldUpdateGPUData = false;
			return;
		}
	}

	this->updateConstantBuffer();
}

void Entity::updateConstantBuffer() {
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
	this->gEntityPSConstantBuffer.color = dx::XMFLOAT3(
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

void Entity::Reset() {
	this->dataChanged = false;
	this->shouldUpdateGPUData = false;
}

void Entity::attachTextureAndSampler(Texture* texture, TextureSampler* textureSampler){
	this->texture = texture;
	this->textureSampler = textureSampler;
	this->useTexture = true;

	this->dataChanged = true;
}

void Entity::detachTextureAndSampler() {
	this->texture = NULL;
	this->textureSampler = NULL;
	this->useTexture = false;

	this->dataChanged = true;
}

bool Entity::attachMesh(Mesh* mesh) {
	if (this->mesh == NULL) {
		this->mesh = mesh;
		return true;
	}
	return false;
}

void Entity::setColor(Color color) {
	this->entityMaterial.color = color;
	this->dataChanged = true;
}

void Entity::Translate(Vector3 translationVector){
	this->gPosition.x += translationVector.x;
	this->gPosition.y += translationVector.y;
	this->gPosition.z += translationVector.z;
	this->dataChanged = true;
}

void Entity::Rotate(Vector3 rotationVector){
	this->dataChanged = true;
}

void Entity::Scale(Vector3 scalingVector){
	this->gSize.x += scalingVector.x;
	this->gSize.y += scalingVector.y;
	this->gSize.z += scalingVector.z;
	this->dataChanged = true;
}