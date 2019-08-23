#include "BaseEntity.h"
#include "Shapes.h"

PxPhysics* BaseEntity::ppxPhysics;

BaseEntity::~BaseEntity() {
	delete[] this->gVertices;
	delete[] this->gIndices;
	this->useTexture = false;
}

void BaseEntity::attachTextureAndSampler(Texture* texture, TextureSampler* textureSampler, bool clearColor){
	this->texture = texture;
	this->textureSampler = textureSampler;
	this->useTexture = true;

	if (clearColor) {
		this->gColor = Color(1,1,1,1);
	}
}

void BaseEntity::detachTextureAndSampler() {
	this->texture = NULL;
	this->textureSampler = NULL;
	this->useTexture = false;
}

void BaseEntity::Update(bool initial){}
void BaseEntity::updateConstantBuffer(){}
void BaseEntity::gCreateVerticesAndIndices(){}